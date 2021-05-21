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
    int thisPlayerID = 0;
    int seq_nr;
    //Messages
   

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


        // Resolve the server address and port
        int iResult = getaddrinfo(IPaddr.c_str(), port.c_str(), &hints, &result);
        if (iResult != 0) {
            printf("getaddrinfo failed: %d\n", iResult);
            WSACleanup();
            return;
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
            return;
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
            return;
        }
        cout << "Connected\n";

        createJoinMsg();

        cout << "End of constructor\n";
    };

    void recieveInformation()
    {
        char recvBuf[BUFLEN];
        while(true)
        {
            int iRecieve = recv(TCPSocket,recvBuf,BUFLEN,0);
            if(iRecieve == SOCKET_ERROR)
            {
                std::cout << "recv() error " << WSAGetLastError() << std::endl;
            }
            else
            {
                //remove the struct from the buffer
                MsgHead* head = (MsgHead*)recvBuf;
                std::cout << "Got packet with playerID: " << head->id << std::endl;
                cout << "seq_nr : " << head->seq_no << endl;
                seq_nr = head->seq_no;
                cout << "type : " << head->type << endl;
                // when player joins setup id
                if (head->type == Join) 
                {
                    if (thisPlayerID == 0) 
                    {
                        std::cout << "Player join the game\n";
                        thisPlayerID = head->id;
                    }
                }
            }
        }
    }

    void receiveInfo(int seqNum) {
	char recvBuf[BUFLEN];
	int receive;

	while (true) {

		receive = recv(TCPSocket, recvBuf, BUFLEN, 0);
        if(receive == SOCKET_ERROR)
        {
            cout << "fail";
        }
		MsgHead* head = (MsgHead*)recvBuf;
		std::cout << "Got packet with playerID: " << head->id << std::endl;
        cout << "type : " << head->seq_no << endl;
		int ID = head->id;

		if (head->type == Join) {
			
			if (thisPlayerID == 0) {
				std::cout << "Join";
				thisPlayerID = head->id;
			}
			
			
		}

		if (head->type == Leave) {
			std::cout << "Leave";


		}
        /*
		if (head->type == Event) {
			std::cout << "Event";

			if (head->type == Move) {
				std::cout << "Move";

			}


		}*/

		

		/*if (head->type == Change) {
			std::cout << "Change" << std::endl;
			ChangeMsg* change = (ChangeMsg*)head;
			NewPlayerPositionMsg* NPPmsg = (NewPlayerPositionMsg*)head;
			if (change->type == NewPlayerPosition) {
				std::cout << "NewPlayerPosition" << std::endl;
 
				
				int x, y;
				
				x = NPPmsg->pos.x;
				y = NPPmsg->pos.y;
				std::cout << "Player " << head->id << " is now at position x=" << x - 100 << " and y=" << y - 100 << std::endl;
				field[x][y] = NPPmsg->msg.head.id;
				std::string color;
				switch (NPPmsg->msg.head.id) {

				case 0:
					color = "white";
					break;

				case 1:
					color = "blue";
					break;

				case 2:
					color = "green";
					break;

				case 3:
					color = "red";
					break;
				case 4:
					color = "yellow";
					break;
				case 5:
					color = "magenta";
					break;

				case 6:
					color = "orange";
					break;
				case 7:
					color = "pink";
					break;
				case 8:
					color = "cyan";
					break;

				}
				std::string GUImsg;
				GUImsg = std::to_string(x) + "," + std::to_string(y) + "," + color;
				updateGUI(GUISock, ID, GUI, GUImsg);
				
				
			}*/
            /*
			if (change->type == NewPlayer) {
				std::cout << "NewPlayer" << std::endl;
				
				
			}*/
            /*
			if (change->type == PlayerLeave) {
				std::cout << "PlayerLeave" << std::endl;
				
				if (head->id == thisPlayerID) {
					std::cout << "Disconnected from server" << std::endl;
					return;
				}
				std::cout << "Player " << head->id << " left. Removing from board" << std::endl;
				int *remove = getPos(head->id);
				int x = *remove;
				int y = *(remove +1);
				std::cout << x << " . " << y << std::endl;
				
				
				std::string GUImsg;
				GUImsg = std::to_string(x) + "," + std::to_string(y) + ",white";
				updateGUI(GUISock, ID, GUI, GUImsg);
				field[x][y] = 0;
			}*/

		}
	}


    void createJoinMsg()
    {
                JoinMsg join;
                MsgType type;
                type = Join;    //enum msg tpe with value 0 ie join.
                MsgHead head;
                head.length = sizeof(join);
                head.seq_no = 1;
                head.id = 2;
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

    void sendMoveMessage(int seqNr, int clientID, Coordinate posReq)
    {
        ZeroMemory(sendMessage, BUFLEN);

        MoveEvent event;
        EventMsg msg;

        //Head decleration
        MsgHead head;
        head.length = sizeof(sendMessage);
        head.id = clientID;
        head.seq_no = seqNr;
        MsgType type = Event;
        head.type = type;

        msg.head = head;
        msg.type = Move;

        event.event = msg;
        event.dir = posReq;

        //copy the msg to the buffer
        memcpy((void*)sendMessage, (void*)&event, sizeof(event));

        int isent = send(TCPSocket,sendMessage,sizeof(sendMessage),0);
        if(isent == SOCKET_ERROR)
        {
            std::cout << "send() error " << WSAGetLastError() << std::endl;
        }
    }

};