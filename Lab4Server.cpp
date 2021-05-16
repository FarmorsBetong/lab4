#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <iphlpapi.h>
#include <stdio.h>


#pragma comment (lib, "ws2_32.lib")

using namespace std;

int main()
{

    //Winsock startup
    WSADATA wsData;
    WORD ver = MAKEWORD(2,2);
    int wsOK = WSAStartup(ver,&wsData);

    if(wsOK != 0)
    {
        cout << "Cant init winsock, quitting";
        return 1;
    }

    //Socket creation
    SOCKET listnening = socket(AF_INET,SOCK_STREAM,0);
    if(listnening == INVALID_SOCKET)
    {
        cout << "Cant create a socket, quitting";
        return 1;
    }

    //bind the ip and port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(49512);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;

    bind(listnening, (sockaddr*)&hint, sizeof(hint));

    //Setup socket to listen
    listen(listnening,SOMAXCONN);

    //wait for a connection
    sockaddr_in client;
    int clientSize = sizeof(client);

    SOCKET clientSocket = accept(listnening, (sockaddr*)&client, &clientSize);
    if(clientSocket == INVALID_SOCKET)
    {
        cout << "Client socket was not accpeted";
    }

    char host[NI_MAXHOST];      //client hostname
    char service[NI_MAXSERV];   // service (i.e port) the client is connected too

    ZeroMemory(host, NI_MAXHOST);
    ZeroMemory(service,NI_MAXSERV);

    if(getnameinfo((sockaddr*)&client, sizeof(client),host, NI_MAXHOST,service, NI_MAXSERV, 0) == 0)
    {
        cout << host << " connected on port " << service << endl;
    }
    else
    {
        inet_ntoa(client.sin_addr);
        //inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " connected on port" << ntohs(client.sin_port) << endl;
    }

    //close listening socket
    closesocket(listnening);

    // while loop

    char buf[4096];

    while(true)
    {
        ZeroMemory(buf, 4096);

        //wait for client to send data

        int bytesReceived = recv(clientSocket, buf, 4096,0);
        if(bytesReceived == SOCKET_ERROR)
        {
            cout << "Error in recv()";
            break;
        }

        if(bytesReceived == 0)
        {
            cout << "client disconneted";
            break;
        }
        //echo msgs
        send(clientSocket, buf, bytesReceived + 1,0);

        //close socket
        closesocket(clientSocket);

        //cleanwinsock
        WSACleanup();

    }
}