#define MAXNAMELEN 32
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501


#include <iostream>
#include <string>
#include <WinSock2.h>
#include <Windows.h>
#include <Ws2tcpip.h>

#include <iphlpapi.h>
#include <stdio.h>

#pragma comment (lib, "ws2_32.lib")

//#define InetPtonA inet_pton

using namespace std;

//Protocols

enum ObjectDesc
{
Human,
NonHuman,
Vehicle,
StaticObject
};

enum ObjectForm
{
Cube,
Sphere,
Pyramid,
Cone
};

struct Coordinate
{
int x;
int y;
};

enum MsgType
{
Join, // Client joining game at server
Leave, // Client leaving game
Change, // Information to clients
Event, // Information from clients to server
TextMessage // Send text messages to one or all
};

struct MsgHead
{
unsigned int length; // Total length for whole message
unsigned int seq_no; // Sequence number
unsigned int id; // Client ID or 0;
MsgType type; // Type of message
};

int main()
{

    string IPaddr = "130.240.40.7";
    int serverPort = 49152;

    //init winsock
    WSADATA wsData;

    WORD version = MAKEWORD(2,2);

    int wsOk = WSAStartup(version,&wsData);
    if(wsOk != 0)
    {
        cerr << "winsock startup failed" << endl;
        return -1;
    }

    cerr << "wsOk " << wsOk << endl;

    // Fill in a hint structure
    /*
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(serverPort);
    inet_pton(AF_INET, IPaddr.c_str(), &hint.sin_addr);

    */
   struct addrinfo *result = NULL,
                *ptr = NULL,
                hints;

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    #define DEFAULT_PORT "49152"
    cout << "structure created \n";

    // Resolve the server address and port
    int iResult = getaddrinfo(IPaddr.c_str(), DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    cout << "ip and port information converted to bytes\n";

    //create socket
    /*
    SOCKET sock = socket(AF_INET, SOCK_STREAM,0);
    if(sock == INVALID_SOCKET)
    {
        cout << "Can't create a socket quitting";
        WSACleanup();
        return -1;
    }*/

    // Attempt to connect to the first address returned by
    // the call to getaddrinfo
    ptr=result;

    SOCKET sock = INVALID_SOCKET;
    // Create a SOCKET for connecting to server
    sock = socket(ptr->ai_family, ptr->ai_socktype,
        ptr->ai_protocol);

    if (sock == INVALID_SOCKET) {
    printf("Error at socket(): %ld\n", WSAGetLastError());
    freeaddrinfo(result);
    WSACleanup();
    return 1;
    }
     cout << "socket har suttit upp ip o port\n";

    //Connect to the server
    int connResult = connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen);


    //valid
    if(connResult == SOCKET_ERROR)
    {
        cerr << "Cant connect to server, err #" << WSAGetLastError() << endl;
        closesocket(sock);
        sock = INVALID_SOCKET;
        WSACleanup();
    }
    else {
        cout << "Connected\n";
    }

    freeaddrinfo(result);

    if (sock == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }
    cout << "conn gick igenom";

    struct JoinMsg
    {
    MsgHead head;
    ObjectDesc desc;
    ObjectForm form;
    char name[MAXNAMELEN]; // nullterminated!, or empty
    };

    //loop to send and recieve messages
    char buf[4096];
    char* buffer = new char[sizeof(JoinMsg)];
    string userInput;

    do
    {
        //prompt user for some text
        cout << "> ";
        getline(cin, userInput);

        if(userInput.size() > 0)
        {

            int sendResult;
            if(userInput == "1")
            {
                MsgType type;
                type = Join;    //enum msg tpe with value 0 ie join.
                MsgHead head;
                head.length = 4096;
                head.seq_no = 1;
                head.id = 2;
                head.type = type;

                ObjectDesc desc;
                desc = Human;
                ObjectForm form;
                form = Cube;

                JoinMsg join;
                join.head = head;
                join.desc = desc;
                join.form = form;


                // Fixa att packetera ner structen till en char* buffer för att kunna skicka den till server
                //memcpy(buffer, &join, sizeof(join));

                sendResult = send(sock, join, sizeof(join), 0);
            }
            //send text
            sendResult = send(sock,userInput.c_str(), userInput.size()+1,0);

            if(sendResult != SOCKET_ERROR)
            {
                //wait for response
                ZeroMemory(buf, 4096);
                int byteReceived = recv(sock, buf, 4096, 0);
                //if we get something
                if(byteReceived  > 0) {
                    //echo response
                    cout << "Server " << string(buf, 0, byteReceived) << endl;
                }
            }
        }
        //wait for response
        //echo respons
    } while(userInput.size() > 0);


    //shutdown winsock
    closesocket(sock);
    WSACleanup();

    return 1;
}