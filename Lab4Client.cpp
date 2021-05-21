#define MAXNAMELEN 32
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501



#include "GameProtocol.cpp"
#include "Playground.cpp"
#include "ServerConnection.cpp"
#include <iostream>
#include <string>
#include <WinSock2.h>
#include <Windows.h>
#include <Ws2tcpip.h>
#include <chrono>
#include <thread>
#include <iphlpapi.h>
#include <stdio.h>
#include <conio.h>


#pragma comment (lib, "ws2_32.lib")

//#define InetPtonA inet_pton
#define BUFLEN 512	//Max length of buffer
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

using namespace std;

Playground gameClient;

bool gameStarted = false;

int playerID;
int seqNr;
Coordinate pos;
string playerColor;


// test func
void keyInput()
{
    int c = 0;
    int y;
    int x;
    while(1)
    {
        switch(c = getch())
        {
            case KEY_UP:
                cout << "Up\n";
                y = pos.y -1;
                if(y > -1)
                {
                    pos.y = y;
                    gameClient.move(pos,playerColor);
                }
                break;
            case KEY_DOWN:
                cout << "down\n";
                y = pos.y + 1;
                if(y < 100)
                {
                    pos.y = y;
                    gameClient.move(pos,playerColor);
                }
                break;
            case KEY_LEFT:
                cout << "left\n";
                x = pos.x -1;
                if(x > -1)
                {
                    pos.x = x;
                    gameClient.move(pos,playerColor);
                }
                break;
            case KEY_RIGHT:
                cout << "right\n";
                x = pos.x + 1;
                if(x < 100)
                {
                    pos.x = x;
                    gameClient.move(pos,playerColor);
                }
                break;
            default:
                break;
        }
    }
}

/*
void sendChangePos()
{
    int c = 0;
    int y;
    int x;
    while(1)
    {
        switch(c = getch())
        {
            case KEY_UP:
                cout << "Up\n";
                y = pos.y -1;
                if(y > -1)
                {
                    pos.y = y;
                    gameClient.move(pos,playerColor);
                }
                break;
            case KEY_DOWN:
                cout << "down\n";
                y = pos.y + 1;
                if(y < 100)
                {
                    pos.y = y;
                    gameClient.move(pos,playerColor);
                }
                break;
            case KEY_LEFT:
                cout << "left\n";
                x = pos.x -1;
                if(x > -1)
                {
                    pos.x = x;
                    gameClient.move(pos,playerColor);
                }
                break;
            case KEY_RIGHT:
                cout << "right\n";
                x = pos.x + 1;
                if(x < 100)
                {
                    pos.x = x;
                    gameClient.move(pos,playerColor);
                }
                break;
            default:
                break;
        }
    }
}
*/
int main()
{

    gameClient.clearBoard();
    pos.x = 3;
    pos.y = 3;
    playerColor = "red";
    gameClient.placePlayer(pos,playerColor);
    gameStarted = true;

    keyInput();
    //std::thread first (keyInput);


    //first.join();

    //Sleep(2000);
    //gameClient.move(3,3,1,1,"red");


    //ServerConnection con;


    return 1;
}