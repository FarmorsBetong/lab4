//Author Robin Olofsson, roblof-8 . 2021-05-10
// Version 1.0

#include "Server.h"
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <errno.h>
#include <sys/time.h>
//#include <unordered_map>
#include <typeinfo>
#include <queue>

#define BUFLEN 512
#define PORT 49152
#define MAPSIZE 201
#define MAXCLIENTS 7

using namespace std;

int client_sockets[MAXCLIENTS][4]; // array that stores client socket {ID,FD, x,y}
int map[MAPSIZE][MAPSIZE];


// map[][], playerID, new position, a, b are previous position
// sets the position of a player and removes earlier position
void changePlayerPosition(int map[MAPSIZE][MAPSIZE], int ID, int pos[], int a, int b) 
{
	cout << "changing player position\n";
	int x = pos[0]; 
	int y = pos[1];
        if (x == 500 || y == 500) return;
	//reset previous position
        map[x-a][y-b] = 0;
	
	//set current position at the sock Desc
        client_sockets[ID][2] = x;
        client_sockets[ID][3] = y;
	
	//save the new position on the map
        map[x][y] = ID;
        return;
}

/*
	Inputs (map[][] and player ID)
	returns the position of the player with the ID sent as parameter.
*/
int * getPosition(int field[MAPSIZE][MAPSIZE], int ID)
{
    static int pos [2];
    pos[0] = client_sockets[ID][2];
    pos[1] = client_sockets[ID][3];
    return pos;
}

int main() 
{
	
	cout << "Server up and running\n";

	//Variables
	int maxSock, activity, valread, addrlen, playerID;

	//sockets
	int sockDesc, clientSocket, listeningSocket;
	
	//buffers
    	char sendBuf[BUFLEN];
	char recvBuf[BUFLEN];

	
	//hint
	sockaddr_in hints;

	//setup move event queue
	std::queue <MoveEvent*> moveQueue;

	//read file descrptor
	fd_set readfds;


	//init client sockets
	//initialize clientsockets to zeros and IDS
	for(int i = 1; i< MAXCLIENTS; i++)
	{
		client_sockets[i][1] = 0;
		client_sockets[i][0] = i;
	}

	for(int i = 0; i<MAPSIZE; i++)
	{
		for(int j = 0; j<MAPSIZE; j++)
		{
		    map[i][j] = 0;
		}
	}
	
	
	//Create listning socket IPv4, TCP, IP
	if((listeningSocket = socket(AF_INET, SOCK_STREAM,0)) == -1)
	{
		cout << "Can't create listening socket, error value : " << errno << endl;
		return -1;
	}

	//Create socket hint structure
	hints.sin_family = AF_INET;
	hints.sin_port = htons(PORT); //host to network endians
	//hints.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_pton(AF_INET, "0.0.0.0", &hints.sin_addr);

	
	//bind socket
	if(bind(listeningSocket, (sockaddr *)&hints, sizeof(hints))  < 0)
	{
		printf("Failed to bind socket %d", errno);
		return -1;
	}
	printf("Server listening \n");
	

	//Setup socket to listen 
	if(listen(listeningSocket, MAXCLIENTS) != 0)
	{
		printf("Listen() failed");
		return -1;
	}

	//hint strucutre for client
	sockaddr_in client;
	addrlen = sizeof(hints);
	socklen_t clientSize = sizeof(client);
    
 	
    	char host[NI_MAXHOST];      // Client's remote name
    	char service[NI_MAXSERV];   // Service (i.e. port) the client is connect on
 

	std::fill_n(host, NI_MAXHOST, 0);
    	std::fill_n(service, NI_MAXSERV, 0);

	
	while(true)
	{
		//init file Descriptor
		FD_ZERO(&readfds);
        
		//add the listening sock to the set
		FD_SET(listeningSocket, &readfds);
		maxSock = listeningSocket;

		//add the clients to the set
        	for(int i = 1; i < MAXCLIENTS; i++)
		{
            		//the socket filedescriptor gets put in the set
            		sockDesc = client_sockets[i][1];

            		if(sockDesc > 0 )
			{
		               FD_SET(sockDesc, &readfds); 
            		} 
            		

			if(sockDesc > maxSock)
			{ //this is for the select function later, need the 
                           
                		maxSock = sockDesc;
            		} 
        	}

		//wait for an activity on one of the sockets
		activity = select(maxSock + 1, &readfds, NULL, NULL, NULL);
		cout << "activity: " << activity << endl;
		if((activity < 0) && (errno != EINTR))
		{
		    cout << "selection error" << endl;
		}

		// if join event
		if(FD_ISSET(listeningSocket, &readfds))
		{
			clientSocket = accept(listeningSocket, (sockaddr*)&client, &clientSize);

			if(clientSocket < 0)
			{
			        cout << "accept error" << endl;
		        	return -1;
		    	}
			cout << "client accepted \n";

			 //add a new socket to the list if there is space
            		for(int i = 1; i< MAXCLIENTS; i++)
			{
				//When we find the first empty slot in the client list use it and create a new player
				if(client_sockets[i][1] == 0)
				{
					int fd;  //FILE DESC                 
				 	client_sockets[i][1] = clientSocket;
					fd = client_sockets[i][1];
				        playerID = i;
				        cout << "Adding player with id: " << i << " to the socket list" << endl;
				        std::fill_n(recvBuf, BUFLEN, 0);
				        int bytesReceived = recv(fd, recvBuf, sizeof(recvBuf), 0);

					//start of creation of join msg
				        MsgHead* head = (MsgHead*)recvBuf;
				        cout << "Join, ID = " << playerID << endl;
				        JoinMsg join;
				        join.head.type = Join;         

				        cout << "sockDesc is " << client_sockets[i][1] << ". id is " << client_sockets[i][0] << endl;           

				    	join.head.id = playerID;
				    	join.head.seq_no = head->seq_no + 1;
				    	join.head.length = sizeof(join);

					memcpy((void*)sendBuf, (void*)&join, sizeof(join));

					send(fd, sendBuf, sizeof(sendBuf), 0);
					cout << "sending Join msg" << endl;

					std::fill_n(sendBuf, BUFLEN, 0);


					//start of creation of new player msg
					NewPlayerMsg NPmsg;
					NPmsg.msg.type = NewPlayer;
					NPmsg.msg.head.seq_no = head->seq_no + 1;
					NPmsg.msg.head.id = playerID;
					NPmsg.msg.head.type = Change;
					NPmsg.msg.head.length = sizeof(NPmsg);

					memcpy((void*)sendBuf, (void*)&NPmsg, sizeof(NPmsg));

					// loop through the list of clients and send out the NPmsg to all clients
					for(int i = 1; i< MAXCLIENTS; i++)
					{
						if(client_sockets[i][1] != 0)
						{
							fd = client_sockets[i][1];
							send(fd, sendBuf, sizeof(sendBuf), 0);
							cout << "Sending NewPlayer to  playerID/sd " << playerID << "/" << fd << std::endl;
						    
						}
					}
					cout << "sent NPPmsgs to all clients!\n";
					
					std::fill_n(sendBuf, BUFLEN, 0);


					// start of new player position message
				        NewPlayerPositionMsg NPPmsg;               
				        NPPmsg.msg.type = NewPlayerPosition;                
				        NPPmsg.msg.head.seq_no = head->seq_no + 1;               
				        NPPmsg.msg.head.id = playerID;               
				        NPPmsg.msg.head.type = Change;
				                  
				        NPPmsg.msg.head.length = sizeof(NPPmsg);
				       
				    
				        static int startPos[] = {0,0};
				        while(map[startPos[0]][startPos[1]] != 0)
					{
					        startPos[0]++;
					        if(startPos[0] >198)
						{
				            		startPos[0] = 0;
				            		startPos[1]++;
				            		if(startPos[1] > 198)
							{
				                		startPos[1] = 0;
						        }
				        	}
				    	}
				        NPPmsg.pos.x = startPos[0];
				        NPPmsg.pos.y = startPos[1];
				        memcpy((void*)sendBuf, (void*)&NPPmsg, sizeof(NPPmsg));

					//add a function that adds the player to the board..
					changePlayerPosition(map, playerID,startPos,0,0);

					// Go through the list of connected clients and send out move msgses
					for(int i = 1; i< MAXCLIENTS; i++)
					{
                        			if(client_sockets[i][1] != 0)
						{
			                            fd = client_sockets[i][1];
			                            send(fd, sendBuf, sizeof(sendBuf), 0);
			                            cout << "Sending MoveMsg to playerID " << playerID  << endl;
                       				 }
                    			}

					//reset send buffer
					std::fill_n(sendBuf, BUFLEN, 0);
					break;

				}//end of adding new client to sockDesc list
	
			}//end of loop adding new a client
			
			continue;
		}//end of Join

		//loop through client IDS	
		for(int i = 1; i < MAXCLIENTS; i++)
		{
			sockDesc = client_sockets[i][1];
			
			//returns the socket Desc that recieves a msg
			if(FD_ISSET(sockDesc, &readfds))
			{
				//reset buffer
				std::fill_n(recvBuf, BUFLEN, 0);
				int bytesReceived = recv(sockDesc, recvBuf, sizeof(recvBuf), 0);
				MsgHead* head = (MsgHead*)recvBuf;

				//cases of msgs recv
				
				switch(head->type)
				{
				case 1:
					    cout << "Leave msg aquired" << endl;
					    playerID = head->id;
					    if(playerID == 0)
					    {
						break;
					    }

					    cout << "client ID: " << playerID << endl;
					    sockDesc = client_sockets[playerID][1];                    
					    int *pos;
					
  					    //get current position of the player
					    pos = getPosition(map, playerID);

					    //Create a player leave msg and send back
					    PlayerLeaveMsg leave;
					    leave.msg.type = PlayerLeave;
					    leave.msg.head.id = playerID;
					    leave.msg.head.length = sizeof(leave);

					    memcpy((void*)sendBuf, (void*)&leave, sizeof(leave));
					    
					    //send to all clients
					    for(int i = 1; i< MAXCLIENTS; i++)
					    {
						    int fd;
						    if(client_sockets[i][1] != 0)
						    {
						        fd = client_sockets[i][1];
						        send(fd, sendBuf, sizeof(sendBuf), 0);
						    }
					    }
					    //reset send buffer
					    std::fill_n(sendBuf, BUFLEN, 0);
					    
					    //set the socket desc = 0
					    client_sockets[playerID][1] = 0;
					    changePlayerPosition(map, 0, pos, 0, 0 );

					    //returns the ip and port of the connected host from the socket that was used to connect the user.
					    getpeername(sockDesc, (struct sockaddr*)&hints, (socklen_t*)&addrlen);
					    cout << "Client disconnected with the ip: " << inet_ntoa(hints.sin_addr) << " and port number : " << ntohs(hints.sin_port) << endl;
						  
					    //closes the socket
					    close(sockDesc);                        
					    break;
				case 3:
					 cout << "Event recieved \n" << endl;
					 MoveEvent* ME = (MoveEvent*)recvBuf;
					 
					 //push up the event on to the stack
					 moveQueue.push(ME);
					 break;			
				}//end of switch case

				

				// As long as we have events in the queue pop them from queue and start answering msgs until there is no more events in queue.				
				while(moveQueue.size() > 0)
				{
					cout << "We recieved an event! \n";
					MoveEvent* ME = moveQueue.front();
                			moveQueue.pop();
                			EventMsg* event = (EventMsg*)ME;

					//what type of msg
					if(event->type == Move)
					{
						cout << "Move event\n";

						//get coordinates
						int x = ME->pos.x;
						int y = ME->pos.y;
						
						//check wether the coordinates recieved are in the right protocol.
						if(x > 1 || y > 1 || x < -1 || y < -1)
						{
						    continue;
						}

						playerID = head->id;
                        	
						//get current postion
						int *pos;
						pos = getPosition(map, playerID);

						cout << "playerID: " << playerID << endl;
						//check if its out of the map
						if(x + *(pos+0) < 0 || y + *(pos+1) < 0 || x + *(pos+0) > 200 || y + *(pos+1)> 200)
						{
						    break;
						}

						int newPos[2] = {*(pos+0)+x, *(pos+1)+y};

						//if new pos is taken
						if(map[newPos[0]][newPos[1]] != 0)
						{
						    break;
						}

						//Create new player position msg
						NewPlayerPositionMsg NPPmsg;                  
						NPPmsg.msg.type = NewPlayerPosition;   
						NPPmsg.msg.head.seq_no = head->seq_no + 1; 
						NPPmsg.msg.head.id = head->id;            
						NPPmsg.msg.head.type = Change;
						                            
						//send a message of where the players new location is
						NPPmsg.pos.x = *(pos+0)+x;
						NPPmsg.pos.y = *(pos+1)+y;              
						NPPmsg.msg.head.length = sizeof(NPPmsg);
						cout << "Sending Move " << "x = " << NPPmsg.pos.x - 100 << " , y = " << NPPmsg.pos.y - 100 << ". The player " << head->id << " moved" << endl;
						
						memcpy((void*)sendBuf, (void*)&NPPmsg, sizeof(NPPmsg));
					
						changePlayerPosition(map, playerID, newPos, x, y);
						
						//send out move player msgs to all connected clients.
						for(int i = 1; i< MAXCLIENTS; i++)
						{
						    int fd;
						    if(client_sockets[i][1] != 0)
						    {
						        fd = client_sockets[i][1];
						        cout << "Sending Move to  playerID " << client_sockets[i][0] <<  endl;
						        send(fd, sendBuf, sizeof(sendBuf), 0);
						    }
						}
						//reset buffer
						std::fill_n(sendBuf, BUFLEN, 0);						
						
					}//end of move event

				}// end of while(queue)

				for(int i = 1; i< MAXCLIENTS; i++)
				{
		        		cout << "ID " << client_sockets[i][0] << " holds fileDescriptor : " << client_sockets[i][1] <<  endl;
		    		}			
			}//end of FD_ISSET()

		}//loop through all clients
		
	}//end of while
   
return 0;	
}//end of main