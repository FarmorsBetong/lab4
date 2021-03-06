#define MAXNAMELEN 32
#define DEFAULT_PORT 49152
#define BUFLEN 512	//Max length of buffer
#define MAPSIZE 101

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define L 108
#define MAX_NR_OF_PLAYERS 7

#include "GameProtocol.cpp"
#include "Playground.cpp"
#include <iostream>
#include <string>
#include <WinSock2.h>
#include <Windows.h>
#include <Ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <chrono>
#include <process.h>
#include <conio.h>

#define InetPtonA inet_pton

using namespace std;

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
    int thisPlayerID = 0;
    string color;
    int seq_nr;

    Coordinate pos;
    int clientID;
    int map[MAPSIZE][MAPSIZE];

    bool gameStarted = false;
    bool quit = false;

    //GUI client
    Playground gameClient;

//#pragma comment (lib, "ws2_32.lib")

/*
Coordinate getGridPos(int id)
{
    for(int x = 0; x < MAPSIZE; x++)
    {
        for(int y = 0; y < MAPSIZE; y++)
        {
            if(map[x][y] == id)
            {
                pos.x = x;
                pos.y = y;
                return pos;
            }
        }
    }
}
*/

/**
 * @brief  recv() recieves messages from server, intepreter them by the protocol that is set between them  
 * and acts acording to it. The msgses are : New player, Change player position, or leave. Thread 1 calls this function
 * @param data 
 */

void recvMessages(void* data)
{
        while(true)
        {
            ZeroMemory(recvMessage,BUFLEN);
            int iRecieve = recv(TCPSocket,recvMessage,BUFLEN,0);
            if(iRecieve == SOCKET_ERROR)
            {
                std::cout << "recv() error " << WSAGetLastError() << std::endl;
            }
            else
            {
                //remove the struct from the buffer
                MsgHead* head = (MsgHead*)recvMessage;
                cout << "Got packet with playerID: " << head->id << endl;
                cout << "seq_nr : " << head->seq_no << endl;
                seq_nr = head->seq_no;
                cout << "type : " << head->type << endl;
                // when player joins setup id
                if (head->type == Join)
                {
                    if (thisPlayerID == 0)
                    {
                        cout << "Player join the game\n";
                        thisPlayerID = head->id;
                    }
                }

                if(head->type == Change)
                {
                    cout << "Change\n";
                    ChangeMsg* change = (ChangeMsg*)head;
                    NewPlayerPositionMsg* NewPmsg = (NewPlayerPositionMsg*)head;

                    //ID of msg
                    int ID = NewPmsg->msg.head.id;

                    if(change->type == NewPlayerPosition)
                    {
                        cout << "New player position msg\n";

                        cout << "position got back from server " << NewPmsg->pos.x << ":" << NewPmsg->pos.y << "\n";

                        switch (ID) 
                        {
                            case 0:
                                color = "white";
                                break;

                            case 1:
                                color = "green";
                                break;

                            case 2:
                                color = "red";
                                break;
                            case 3:
                                color = "yellow";
                                break;
                            case 4:
                                color = "black";
                                break;

                            case 5:
                                color = "orange";
                                break;
                            case 6:
                                color = "pink";
                                break;
                            case 7:
                                color = "grey";
                                break;
                            default:
                                break;
                            }

                            pos.x = NewPmsg->pos.x;
                            pos.y = NewPmsg->pos.y;

                            gameClient.move(pos, color, ID);
                    }


                    if(change->type == NewPlayer)
                    {
                        cout << "New Player game started \n";

                        gameStarted = true;
                    }

                    if(change->type == PlayerLeave)
                    {
                        cout << "player " << head->id << " is leaving\n";
                    }
                }
            }
        }
}

void createJoinMsg()
    {
                JoinMsg join;
                MsgType type;
                type = Join;    //enum msg tpe with value 0 ie join.
                MsgHead head;
                head.length = sizeof(join);
                head.seq_no = 0;
                head.id = 0;
                head.type = type;

                ObjectDesc desc;
                desc = Human;
                ObjectForm form;
                form = Cube;

                join.head = head;
                join.desc = desc;
                join.form = form;

                 memcpy((void*)sendMessage, (void*)&join, sizeof(join));
                 int isent = send(TCPSocket,sendMessage,sizeof(sendMessage),0);
                 if(isent == SOCKET_ERROR)
                 {
                     std::cout << "send() error " << WSAGetLastError() << std::endl;
                 }
                 std::cout << "sending join msg" << std::endl;
    }




    void sendMoveMessage(Coordinate posReq)
    {
        cout << "sendMove() runs\n";

        ZeroMemory(sendMessage, BUFLEN);
        MoveEvent moveMsg;
        moveMsg.pos.x = posReq.x;
        moveMsg.pos.y = posReq.y;

        moveMsg.event.head.type = Event;
        moveMsg.event.head.id = thisPlayerID;
        moveMsg.event.type = Move;
        moveMsg.event.head.length = sizeof(moveMsg);
        moveMsg.event.head.seq_no = seq_nr +1;


        //copy the msg to the buffer
        memcpy((void*)sendMessage, (void*)&moveMsg, sizeof(moveMsg));

        int isent = send(TCPSocket,sendMessage,sizeof(sendMessage),0);
        if(isent == SOCKET_ERROR)
        {
            std::cout << "send() error " << WSAGetLastError() << std::endl;
        }

        cout << "new pos has been sent\n";
    }

    /**
     * @brief  sendLeave() gets called if the player wants to leave the game.
     * The function creates a leave msg for the server and sends it, clears the board client GUI,
     * and turn the game flag off and quit.
     * 
     */

    void sendLeave() {

	LeaveMsg leave;
	leave.head.id = thisPlayerID;
	leave.head.seq_no = seq_nr+1;
	leave.head.type = Leave;
	leave.head.length = sizeof(leave);
	memcpy((void*)sendMessage, (void*)&leave, sizeof(leave));

	int sent = send(TCPSocket, sendMessage, sizeof(sendMessage), 0);

    if(sent == SOCKET_ERROR)
    {
        cout << "send leave message failed";
        WSACleanup();
    }
    gameClient.clearBoard();
    gameClient.closeSocket();
    gameStarted = false;
    quit = true;
}

    /**
     * @brief KeyInput() acts on user input from keyboard. Depending on the input, it creates and sends
     * move position msgs to the server and client GUI. If the player wants to leave it calls leave funcs. Thread 2 runs this function.
     * 
     * @param data 
     */
    void keyInput(void* data)
    {
        int c = 0;
        int y;
        int x;
        Coordinate newPos;
        while(1)
        {
            switch(c = getch())
            {
                case KEY_UP:
                    cout << "Up\n";
                    y = pos.y -1;
                    if(y > -1)
                    {
                        newPos.x = 0;
                        newPos.y = -1;
                        sendMoveMessage(newPos);
                    }
                    break;
                case KEY_DOWN:
                    cout << "down\n";
                    y = pos.y + 1;
                    if(y < 201)
                    {
                        newPos.x = 0;
                        newPos.y = 1;
                        sendMoveMessage(newPos);
                    }
                    break;
                case KEY_LEFT:
                    cout << "left\n";
                    x = pos.x -1;
                    if(x > -1)
                    {
                        newPos.x = -1;
                        newPos.y = 0;
                        sendMoveMessage(newPos);
                    }
                    break;
                case KEY_RIGHT:
                    cout << "right\n";
                    x = pos.x + 1;
                    if(x < 201)
                    {
                        newPos.x = 1;
                        newPos.y = 0;
                        sendMoveMessage(newPos);
                    }
                    break;
                case L:
                    cout << "Leave req \n";
                    sendLeave();
                    break;

                default:
                    break;
            }
        }
    }


int main()
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


        // Resolve the server address and port
        int iResult = getaddrinfo(IPaddr.c_str(), port.c_str(), &hints, &result);
        if (iResult != 0) {
            printf("getaddrinfo failed: %d\n", iResult);
            WSACleanup();
            return 0;
        }

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
            return 0;
        }
        cout << "TCP socket created\n";
        //Connect to the server
        int connResult = connect(TCPSocket, ptr->ai_addr, (int)ptr->ai_addrlen);


        //valid
        if(connResult == SOCKET_ERROR)
        {
            cerr << "Cant connect to server, err #" << WSAGetLastError() << endl;
            closesocket(TCPSocket);
            TCPSocket = INVALID_SOCKET;
            WSACleanup();
        }

        freeaddrinfo(result);

        if (TCPSocket == INVALID_SOCKET) {
            printf("Unable to connect to server!\n");
            WSACleanup();
            return 0;
        }
        cout << "Connected\n";

        createJoinMsg();

         pos.x = 0;
         pos.y = 0;

        gameClient.clearBoard();

        HANDLE myHandleRecv, myHandleSend;

        myHandleRecv =(HANDLE)_beginthread(&recvMessages,0,0);

        myHandleSend = (HANDLE)_beginthread(&keyInput,0,0);

        while(!quit)
        {

        }
        closesocket(TCPSocket);
        WSACleanup();
        return 0;
}