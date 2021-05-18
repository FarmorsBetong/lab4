#define MAXNAMELEN 32
#define DEFAULT_PORT 49152
#define BUFLEN 512	//Max length of buffer

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501

#include <iostream>
#include <string>
#include <WinSock2.h>
#include <Windows.h>
#include <Ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#define InetPtonA inet_pton

using namespace std;


#pragma comment (lib, "ws2_32.lib")

class ServerConnection
{
    //Attributes
    public:

    //Socket creation;
    SOCKET TCPSocket;

    //hint creation
    struct addrinfo *result = NULL,
                *ptr = NULL,
                serverHint;

    //create send and recv buffers  
    char sendMessage[512];
    int sendlen = strlen(sendMessage)+1;
    char recvMessage[512];
    int recvlen = strlen(recvMessage)+1;

    string IPaddr = "130.240.40.7";
    string port = "49152";

    public:
    //constructor
    ServerConnection()
    {
        //Winsock startup
        WSADATA wsData;
        WORD version = MAKEWORD(2,2);

        int wsOk = WSAStartup(version,&wsData);
        if(wsOk != 0)
        {
            std::cout << "winsock startup failed" << std::endl;
        }

        struct addrinfo *result = NULL,
                *ptr = NULL,
                hints;

        ZeroMemory( &hints, sizeof(hints) );
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        cout << "structure created \n";

        // Resolve the server address and port
        int iResult = getaddrinfo(IPaddr.c_str(), port.c_str(), &hints, &result);
        if (iResult != 0) {
            printf("getaddrinfo failed: %d\n", iResult);
            WSACleanup();
            return;
        }
        cout << "ip and port information converted to bytes\n";


        // Attempt to connect to the first address returned by
        // the call to getaddrinfo
        ptr=result;

       
        // Create a SOCKET for connecting to server
        TCPSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);

        if (TCPSocket == INVALID_SOCKET) 
        {
            printf("Error at socket(): %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            return;
        }
        cout << "ip and port setup\n";

        //Connect to the server
        int connResult = connect(TCPSocket, ptr->ai_addr, (int)ptr->ai_addrlen);


        //valid
        if(connResult == SOCKET_ERROR)
        {
            cerr << "Cant connect to server, err #" << WSAGetLastError() << endl;
            closesocket(sock);
            TCPSocket = INVALID_SOCKET;
            WSACleanup();
        }

        freeaddrinfo(result);

        if (TCPSocket == INVALID_SOCKET) {
            printf("Unable to connect to server!\n");
            WSACleanup();
            return;
        }
        cout << "Connected";

        int iRecieve = recv(TCPSocket,)
    };

};