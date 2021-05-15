

#include <iostream>
#include <string>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <windows.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment (lib, "ws2_32.lib")

#define InetPtonA inet_pton

using namespace std;

int main()
{

    string IPaddr = "130.240.40.7";
    int serverPort = 49152;

    cout << "test";
    //init winsock
    WSADATA wsData;

    WORD version = MAKEWORD(2,2);

    int wsOk = WSAStartup(version,&wsData);
    if(wsOk != 0)
    {
        cerr << "winsock startup failed" << endl;
        return -1;
    }

    cerr << "nya wsOk" << endl;

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

    // Resolve the server address and port
    int iResult = getaddrinfo(IPaddr.c_str(), DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

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
     cout << "socket har suttit upp ip o port";

    //Connect to the server
    int connResult = connect(sock, ptr->ai_addr, ptr->ai_addrlen);

    //valid

    // Connect to server.
    if(connResult = SOCKET_ERROR)
    {
        cerr << "Cant connect to server, err #" << WSAGetLastError() << endl;
        closesocket(sock);
        sock = INVALID_SOCKET;
        WSACleanup();
    }

    // Should really try the next address returned by getaddrinfo
    // if the connect call failed
    // But for this simple example we just free the resources
    // returned by getaddrinfo and print an error message

    freeaddrinfo(result);

    if (sock == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }


    //loop to send and recieve messages
    //char buff[4096];
    //string userInput = "hello";

    //int sendResult = send(sock, userInput.c_str(), userInput.size() + 1,0);


    //close listening socket

    //loop messages between client and server

    //shutdown winsock

    return 1;
}