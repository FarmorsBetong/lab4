#define MAXNAMELEN 32
#define PORT 4445
#define BUFLEN 512	//Max length of buffer
#define SERVER "127.0.0.1"


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


#pragma comment (lib, "ws2:32.lib")


class playground
{
    public:
    SOCKET sock;
    struct sockaddr_in server;
    int slen=sizeof(server);

    struct sockaddr_in clientRecHint;
    int crlen = sizeof(clientRecHint);

    sockaddr_in clientIn;
    int clen=sizeof(clientIn);
    SOCKET recSock;

    public:
    playground()
    {
        //Winsock startup
        WSADATA wsData;
        WORD version = MAKEWORD(2,2);

        int wsOk = WSAStartup(version,&wsData);
        if(wsOk != 0)
        {
            std::cout << "winsock startup failed" << std::endl;
        }

        //setup address structure
        server.sin_family = AF_INET;
        server.sin_port = htons(PORT);
        server.sin_addr.S_un.S_addr = inet_addr(SERVER);

        // Create a SOCKET for connecting to server
        sock = socket(AF_INET,SOCK_DGRAM,0);

        //error check for socket creation
        if (sock == INVALID_SOCKET)
        {
            printf("Error at socket(): %ld\n", WSAGetLastError());
            WSACleanup();
        }

        std::cout << "socket har suttit upp ip o port\n";

    }
    int startSending()
    {
        while(1)
        {
            char buf[BUFLEN];
            std::string userInput;

            //prompt user for some text
            std::cout << ">";
            std::getline(std::cin, userInput);

            //send message
            if(sendto(sock, userInput.c_str() ,strlen(userInput.c_str()), 0 , (struct sockaddr *)&server, slen) == SOCKET_ERROR)
            {
                printf("sendto() failed with error code : %d" , WSAGetLastError());
			    exit(EXIT_FAILURE);
            }
            //hint structure

            server.sin_family = AF_INET;
            server.sin_port = htons(PORT);
            server.sin_addr.S_un.S_addr = inet_addr(SERVER);

            //socket
            if(recSock= socket(AF_INET,SOCK_DGRAM,0) == SOCKET_ERROR)
            {
                std::cout << "rec socket didnt get created";
                WSACleanup();
            }

            /*
            //bind the socket
            if(bind(recSock,(sockaddr *)&server, slen) == SOCKET_ERROR)
            {
                std::cout << "problem binding the socket closing";
                WSACleanup();
            }*/

            ZeroMemory(buf,BUFLEN);

            int bytesIn = recvfrom(recSock,buf,BUFLEN,0, (sockaddr *)&clientIn, &clen);

            if(bytesIn == SOCKET_ERROR)
            {
                std::cout << "Errorr rec from client" << WSAGetLastError << std::endl;
                WSACleanup();
            }

            /*char clientIP[256];
            ZeroMemory(clientIP,256);
            in_addr address;

            if(inet_ntoa(address) == NULL)
            {

            }*/

            std::cout << "msg: " << buf << std::endl;


            //close socket
            closesocket(recSock);
        }
    }
};