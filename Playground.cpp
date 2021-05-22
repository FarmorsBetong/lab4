#define MAXNAMELEN 32
#define PORT 4445
#define BUFLEN 512	//Max length of buffer
#define LOOPBACK "127.0.0.1"


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


#pragma comment (lib, "ws2:32.lib")


class Playground
{
    public:
        //variables
        SOCKET sock;
        sockaddr_in server;
        int slen=sizeof(server);

        sockaddr_in clientRecHint;
        int crlen = sizeof(clientRecHint);

        sockaddr_in clientIn;
        int clen=sizeof(clientIn);
        SOCKET recSock;


        //Winsock startup
        WSADATA wsData;
        WORD version = MAKEWORD(2,2);


        Coordinate pos;
        char buf[BUFLEN];

    public:
        //construtor for con with gui server
        Playground()
        {
            int wsOk = WSAStartup(version,&wsData);
            if(wsOk != 0)
            {
                std::cout << "winsock startup failed" << std::endl;
            }

            //setup address structure
            server.sin_family = AF_INET;
            server.sin_port = htons(PORT);
            server.sin_addr.S_un.S_addr = inet_addr(LOOPBACK);

            // Create a SOCKET for connecting to server
            sock = socket(AF_INET,SOCK_DGRAM,0);

            //error check for socket creation
            if (sock == INVALID_SOCKET)
            {
                printf("Error at socket(): %ld\n", WSAGetLastError());
                WSACleanup();
            }

            std::cout << "UDP socket created \n";
        }

        // Helper function for testing server gui
        int startSending()
        {
            while(1)
            {
                string userInput;

                //prompt user for some text
                cout << ">";
                getline(std::cin, userInput);

                //send message
                if(sendto(sock, userInput.c_str() ,strlen(userInput.c_str()), 0 , (struct sockaddr *)&server, slen) == SOCKET_ERROR)
                {
                    printf("sendto() failed with error code : %d" , WSAGetLastError());
                    exit(EXIT_FAILURE);
                }

                //socket
                if(recSock= socket(AF_INET,SOCK_DGRAM,0) == SOCKET_ERROR)
                {
                    std::cout << "rec socket didnt get created";
                    WSACleanup();
                }


                ZeroMemory(buf,BUFLEN);

                int bytesIn = recvfrom(sock,buf,BUFLEN,0, (sockaddr *)&clientIn, &clen);

                if(bytesIn == SOCKET_ERROR)
                {
                    std::cout << "Errorr rec from client " << WSAGetLastError << std::endl;
                    //WSACleanup();
                }


                std::cout << "msg: " << buf << std::endl;


                //close socket
            }
        }

        // Clears the board
        void clearBoard()
        {
            string clear = "clear";
            //send message
                if(sendto(sock, clear.c_str() ,strlen(clear.c_str()), 0 , (struct sockaddr *)&server, slen) == SOCKET_ERROR)
                {
                    printf("sendto() failed with error code : %d" , WSAGetLastError());
                    exit(EXIT_FAILURE);
                }
                //clear buffer
                ZeroMemory(buf,BUFLEN);

                //recieve information from gui server
                int bytesIn = recvfrom(sock,buf,BUFLEN,0, (sockaddr *)&clientIn, &clen);

                if(bytesIn == SOCKET_ERROR)
                {
                    std::cout << "Errorr rec from client " << WSAGetLastError << std::endl;
                }

                // print out recv msg
                cout << "msg: " << buf << endl;
        }

        // protocol to move character on the server board is x:y:color

        void move(Coordinate newPos, string color)
            {
                string moveFrom =  to_string(pos.x) + ":" + to_string(pos.y) + ":" + "white";

                //send message
                if(sendto(sock, moveFrom.c_str() ,strlen(moveFrom.c_str()), 0 , (struct sockaddr *)&server, slen) == SOCKET_ERROR)
                {
                    printf("sendto() failed with error code : %d" , WSAGetLastError());
                    exit(EXIT_FAILURE);
                    return;
                }
                //clear buffer
                ZeroMemory(buf,BUFLEN);

                //recieve information from gui server
                int bytesIn = recvfrom(sock,buf,BUFLEN,0, (sockaddr *)&clientIn, &clen);

                if(bytesIn == SOCKET_ERROR)
                {
                    std::cout << "Errorr rec from client " << WSAGetLastError << std::endl;
                    return;
                }

                // print out recv msg
                cout << "msg: " << buf << endl;

                string moveTo = to_string(newPos.x) + ":" + to_string(newPos.y) + ":" + color;

                //send message
                if(sendto(sock, moveTo.c_str() ,strlen(moveTo.c_str()), 0 , (struct sockaddr *)&server, slen) == SOCKET_ERROR)
                {
                    printf("sendto() failed with error code : %d" , WSAGetLastError());
                    exit(EXIT_FAILURE);
                    return;
                }
                //clear buffer
                ZeroMemory(buf,BUFLEN);

                //recieve information from gui server
                bytesIn = recvfrom(sock,buf,BUFLEN,0, (sockaddr *)&clientIn, &clen);

                if(bytesIn == SOCKET_ERROR)
                {
                    std::cout << "Errorr rec from client " << WSAGetLastError << std::endl;
                    return;
                }

                // print out recv msg
                cout << "msg: " << buf << endl;
                pos.x = newPos.x;
                pos.y = newPos.y;
            }


            void placePlayer(Coordinate firstPos, string color)
            {
                cout << "placing player on the map\n ";
                char buf[BUFLEN];
                pos.x = firstPos.x;
                pos.y =firstPos.y;
                string place =  to_string(pos.x) + ":" + to_string(pos.y) + ":" + color;

                //send message
                if(sendto(sock, place.c_str() ,strlen(place.c_str()), 0 , (struct sockaddr *)&server, slen) == SOCKET_ERROR)
                {
                    printf("sendto() failed with error code : %d" , WSAGetLastError());
                    exit(EXIT_FAILURE);
                }
                //clear buffer
                ZeroMemory(buf,BUFLEN);

                //recieve information from gui server
                int bytesIn = recvfrom(sock,buf,BUFLEN,0, (sockaddr *)&clientIn, &clen);

                if(bytesIn == SOCKET_ERROR)
                {
                    cout << "Errorr rec from client " << WSAGetLastError << endl;
                }

                // print out recv msg
                cout << "msg: " << buf << endl;
            }
            /*
            // closes the socket and and cleansup winsock
            void closeSocket()
            {
                closeSocket(sock);
                WSACleanup();
                return;
            }*/
};