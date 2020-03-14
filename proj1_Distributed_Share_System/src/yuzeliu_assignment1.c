/**
 * @yuzeliu_assignment1
 * @author  Yuze Liu <yuzeliu@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#include "../include/global.h"
#include "../include/logger.h"


void client();
void server();
void *get_in_addr(struct sockaddr *sa);
void commandAUTHOR();
void commandIP();
void commandPORT();
int commandLogin(char ipaddressG[256], char portNoG[256]);
void delay(int mseconds);

struct clientToShow{
	char hostNameToShow[50];
	char IPToShow[50];
	int portNoToShow;
	int numberofMessageSend;
	int numberofMessageRec;
	int assignedSocket;
	int status; //1 means the client is online, alive; 0 means the client is off line, logout.
};

int blocklist[5][5];
void add_item(struct clientToShow *p,struct clientToShow a,int num_items);
void delete_item(struct clientToShow *p,int num_items, int item);
void swap_item(struct clientToShow *a, struct clientToShow *b);
void sort_item(struct clientToShow *p, int count);

///New added 3/8
int checkBlockListIndex(char ipaddress[256]);
void updateBlockList(int blocker, int blocked);
int checkSocketTogetIndex(struct clientToShow *p, int count, int socketToCheck);
void readBlockList(int blocker, struct clientToShow *p);
void unblockFromBlockList(int unblocker, int unblocked);
void readRelayBuffer(char *relayBuffer);
char localIpforBlock[50];
void getIP();

char *port;
int temp_port;
/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */


int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log(argv[2]);

	/*Clear LOGFILE*/
	fclose(fopen(LOGFILE, "w"));

	/*Start Here*/
	temp_port = atoi(argv[2]);
	port = malloc(sizeof(char)*5);
	sprintf(port,"%d",temp_port);

	if(*argv[1] == 'c'){
		client();
	} else if(*argv[1] == 's'){
		server();
	}
	return 0;
}

/***********************************************************************************************************************/
/****************************************************Begin of Client Part code******************************************/
/***********************************************************************************************************************/
void client(){
	// When a host run as a client, we first fill the structure of the clientToShow for it at the beginning.
	//struct clientToShow clientTableClient[5]; //create a table that save the connect client information
	struct clientToShow liveClientTableClient[5]; //create a table that save the current log-in client information, only login clients
	fd_set command_fds;       //master file descriptor list
	fd_set readCommand_fds;   //temp file descriptor list for select()
	FD_ZERO(&command_fds);
	FD_ZERO(&readCommand_fds);
	int elementinliveClientTableClient = 0;

	int fdmaxClient;

	FD_SET(STDIN_FILENO, &command_fds);
	fdmaxClient = STDIN_FILENO;

	int argc=0;
	char *arg,argv[20][256];

	char temp_command[50]="";
	int sockfdClient;

	for(;;){
		memcpy(&readCommand_fds, &command_fds, sizeof(command_fds));

		if (select(fdmaxClient+1, &readCommand_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(4);
		}
		for(int i =0; i <= fdmaxClient;i++){
			if(FD_ISSET(i,&readCommand_fds)){
				if(i == STDIN_FILENO) {
					char command[1000];
					fgets(command,1000,stdin);
					command[strlen(command)-1] = '\0';//careful with the new line character
					strcpy(temp_command,command);
					argc = 0;
					arg = strtok(temp_command," ");
					while(arg){
						strcpy(argv[argc++], arg);
						arg = strtok(NULL, " ");
					}

					if(strcmp(command, "AUTHOR") == 0 ){
                        cse4589_print_and_log("[%s:SUCCESS]\n",command);
						commandAUTHOR();
                        cse4589_print_and_log("[%s:END]\n",command);
					} else if(strcmp(command, "IP") == 0){
						cse4589_print_and_log("[%s:SUCCESS]\n",command);
						commandIP();
						cse4589_print_and_log("[%s:END]\n",command);
					} else if(strcmp(command,"PORT") == 0){
						cse4589_print_and_log("[%s:SUCCESS]\n",command);
						commandPORT();
						cse4589_print_and_log("[%s:END]\n",command);
					} else if(strcmp(argv[0], "LOGIN") == 0){
						//cse4589_print_and_log("[%s:SUCCESS]\n",command);
						sockfdClient = commandLogin(argv[1],argv[2]);
						if(sockfdClient != -1){
							///
							send(sockfdClient,port,sizeof port,0);
							delay(100);
							///

							FD_SET(sockfdClient,&command_fds);
							if(STDIN_FILENO > sockfdClient){
								fdmaxClient = STDIN_FILENO;
							}else{
								fdmaxClient = sockfdClient;//cse4589_print_and_log("[%s:END]\n","LOGOUT");
							}

							char ListbufFromServer2[1024];
							recv(sockfdClient,ListbufFromServer2, sizeof ListbufFromServer2,0);
							int argc_newline = 0;
							char *arg_newline, argv_newline[20][50];

							arg_newline = strtok(ListbufFromServer2,"\n");
							while(arg_newline){
								strcpy(argv_newline[argc_newline++],arg_newline);
								arg_newline = strtok(NULL,"\n");
							}

							for(int x = 0; x < argc_newline; x++){
								int argc_space = 0;
								char *arg_space, argv_space[20][50];
								arg_space = strtok(argv_newline[x]," ");

								while(arg_space){
									strcpy(argv_space[argc_space++],arg_space);
									arg_space = strtok(NULL," ");
								}

								strcpy(liveClientTableClient[x].hostNameToShow,argv_space[0]);
								strcpy(liveClientTableClient[x].IPToShow,argv_space[1]);
								liveClientTableClient[x].portNoToShow = atoi(argv_space[2]);
							}
							elementinliveClientTableClient = argc_newline;
							///New Added: 3/8/2017
							///After received the List, then handle the buffer data

							char relaybuf[25600]; //256
							memset(relaybuf,'\0',25600); //256
							char signalBuf[50];
							memset(signalBuf,'\0',50);
							recv(sockfdClient,signalBuf, sizeof(signalBuf),0);
							if(strcmp(signalBuf,"Y") == 0){
								delay(100);
								if((recv(sockfdClient,relaybuf, sizeof(relaybuf),0)) > 0){
									char relaySendFromIp[50];
									char relaySendToIp[50];
									char relayMessage[256];
									memset(relayMessage,'\0',256);
									memset(relaySendFromIp, '\0', 50);
									memset(relaySendToIp, '\0',50);
									int relayargc_newline = 0;
									char *relayarg_newline, relayargv_newline[100][256]; //[20][50]
									relayarg_newline = strtok(relaybuf,"\n");
									while(relayarg_newline){
										strcpy(relayargv_newline[relayargc_newline++],relayarg_newline);
										relayarg_newline = strtok(NULL,"\n");
									}
									for(int x = 0; x < relayargc_newline; x++){
										memset(relayMessage,'\0',256);
										memset(relaySendFromIp, '\0', 50);
										memset(relaySendToIp, '\0',50);
										int relayargc_space = 0;
										char *relayarg_space, relayargv_space[256][256];
										relayarg_space = strtok(relayargv_newline[x]," ");
										while(relayarg_space){
											strcpy(relayargv_space[relayargc_space++],relayarg_space);
											relayarg_space = strtok(NULL," ");
										}
										strcpy(relaySendFromIp,relayargv_space[0]);
										strcpy(relaySendToIp, relayargv_space[1]);

										for(int spaceIndex = 2; spaceIndex < relayargc_space; spaceIndex++){
											strcat(relayMessage,relayargv_space[spaceIndex]);
											relayMessage[strlen(relayMessage)] = ' ';
										}
										//relayMessage[strlen(relayMessage) - 1] = '\n';

										//////
										cse4589_print_and_log("[%s:SUCCESS]\n","RECEIVED");
										cse4589_print_and_log("msg from:%s\n[msg]:%s\n",relaySendFromIp,relayMessage);
										cse4589_print_and_log("[%s:END]\n","RECEIVED");
										///////
									}
								}
							}
							cse4589_print_and_log("[%s:END]\n",command);

						} else {
							cse4589_print_and_log("[%s:ERROR]\n","LOGIN");
							cse4589_print_and_log("[%s:END]\n","LOGIN");
						}
//						///
//						send(sockfdClient,port,sizeof port,0);
//						delay(100);
//						///
//
//						FD_SET(sockfdClient,&command_fds);
//						if(STDIN_FILENO > sockfdClient){
//							fdmaxClient = STDIN_FILENO;
//						}else{
//							fdmaxClient = sockfdClient;//cse4589_print_and_log("[%s:END]\n","LOGOUT");
//						}
//
//						char ListbufFromServer2[1024];
//						recv(sockfdClient,ListbufFromServer2, sizeof ListbufFromServer2,0);
//						int argc_newline = 0;
//						char *arg_newline, argv_newline[20][50];
//
//						arg_newline = strtok(ListbufFromServer2,"\n");
//						while(arg_newline){
//							strcpy(argv_newline[argc_newline++],arg_newline);
//							arg_newline = strtok(NULL,"\n");
//						}
//
//						for(int x = 0; x < argc_newline; x++){
//							int argc_space = 0;
//							char *arg_space, argv_space[20][50];
//							arg_space = strtok(argv_newline[x]," ");
//
//							while(arg_space){
//								strcpy(argv_space[argc_space++],arg_space);
//								arg_space = strtok(NULL," ");
//							}
//
//							strcpy(liveClientTableClient[x].hostNameToShow,argv_space[0]);
//							strcpy(liveClientTableClient[x].IPToShow,argv_space[1]);
//							liveClientTableClient[x].portNoToShow = atoi(argv_space[2]);
//						}
//						elementinliveClientTableClient = argc_newline;
//						///New Added: 3/8/2017
//						///After received the List, then handle the buffer data
//
//						char relaybuf[25600]; //256
//						memset(relaybuf,'\0',25600); //256
//						char signalBuf[50];
//						memset(signalBuf,'\0',50);
//						recv(sockfdClient,signalBuf, sizeof(signalBuf),0);
//						if(strcmp(signalBuf,"Y") == 0){
//							delay(100);
//							if((recv(sockfdClient,relaybuf, sizeof(relaybuf),0)) > 0){
//								char relaySendFromIp[50];
//								char relaySendToIp[50];
//								char relayMessage[256];
//								memset(relayMessage,'\0',256);
//								memset(relaySendFromIp, '\0', 50);
//								memset(relaySendToIp, '\0',50);
//								int relayargc_newline = 0;
//								char *relayarg_newline, relayargv_newline[100][256]; //[20][50]
//								relayarg_newline = strtok(relaybuf,"\n");
//								while(relayarg_newline){
//									strcpy(relayargv_newline[relayargc_newline++],relayarg_newline);
//									relayarg_newline = strtok(NULL,"\n");
//								}
//								for(int x = 0; x < relayargc_newline; x++){
//									memset(relayMessage,'\0',256);
//									memset(relaySendFromIp, '\0', 50);
//									memset(relaySendToIp, '\0',50);
//									int relayargc_space = 0;
//									char *relayarg_space, relayargv_space[256][256];
//									relayarg_space = strtok(relayargv_newline[x]," ");
//									while(relayarg_space){
//										strcpy(relayargv_space[relayargc_space++],relayarg_space);
//										relayarg_space = strtok(NULL," ");
//									}
//									strcpy(relaySendFromIp,relayargv_space[0]);
//									strcpy(relaySendToIp, relayargv_space[1]);
//
//									for(int spaceIndex = 2; spaceIndex < relayargc_space; spaceIndex++){
//										strcat(relayMessage,relayargv_space[spaceIndex]);
//										relayMessage[strlen(relayMessage)] = ' ';
//									}
//									//relayMessage[strlen(relayMessage) - 1] = '\n';
//
//									//////
//									cse4589_print_and_log("[%s:SUCCESS]\n","RECEIVED");
//									cse4589_print_and_log("msg from:%s\n[msg]:%s\n",relaySendFromIp,relayMessage);
//									cse4589_print_and_log("[%s:END]\n","RECEIVED");
//									///////
//								}
//							}
//						}
//						cse4589_print_and_log("[%s:END]\n",command);
						///End of buffer data
						//break; ///////////!!!!!!!!!!!!!!!!
					} else if(strcmp(argv[0],"SEND") == 0){
						int boolflghi = 0;
						for(int tempIndex = 0; tempIndex < elementinliveClientTableClient; tempIndex++){
							if(strcmp(liveClientTableClient[tempIndex].IPToShow,argv[1]) == 0){
								cse4589_print_and_log("[%s:SUCCESS]\n","SEND");
								printf("%s%s\n","Client received the command : ",command);
								printf("%d\n",strlen(command)-strlen(argv[0])-strlen(argv[1])-2);
								send(sockfdClient,command,strlen(command),0);
								cse4589_print_and_log("[%s:END]\n","SEND");
								boolflghi = 1;
								break;
							}
						}
						if(boolflghi == 0){
							cse4589_print_and_log("[%s:ERROR]\n","SEND");
							cse4589_print_and_log("[%s:END]\n","SEND");
						}

//						cse4589_print_and_log("[%s:SUCCESS]\n","SEND");
//						printf("%s%s\n","Client received the command : ",command);
//						printf("%d\n",strlen(command)-strlen(argv[0])-strlen(argv[1])-2);
//						send(sockfdClient,command,strlen(command),0);
//						cse4589_print_and_log("[%s:END]\n","SEND");
						//fflush(stdout);
					} else if(strcmp(argv[0],"BROADCAST") == 0){
						cse4589_print_and_log("[%s:SUCCESS]\n","BROADCAST");
						printf("%s%s\n","Client received the command : ",command);
						if(send(sockfdClient, command, strlen(command),0) == -1){
							perror("send");
						}
						cse4589_print_and_log("[%s:END]\n","BROADCAST");
						//fflush(stdout);

					} else if(strcmp(argv[0],"BLOCK") == 0){
						int flagTemo = 0;
						for(int tempi = 0; tempi < elementinliveClientTableClient;tempi++){
							if(strcmp(liveClientTableClient[tempi].IPToShow,argv[1]) == 0){
								//int blockerV2 = 0;
								//int blockedV2 = 0;
//								getIP();
//								blockedV2 = checkBlockListIndex(argv[1]);
//								blockerV2 = checkBlockListIndex(localIpforBlock);
								//cse4589_print_and_log("[%s:SUCCESS]\n","BLOCK");
								printf("%s%s\n","Client received the command : ",command);
								send(sockfdClient, command, strlen(command),0);
								//cse4589_print_and_log("[%s:END]\n","BLOCK");
								flagTemo = 1;
								break;
							}
						}
						if(flagTemo == 0){
							cse4589_print_and_log("[%s:ERROR]\n","BLOCK");
							cse4589_print_and_log("[%s:END]\n","BLOCK");
						}
//						int flagTemo = 0;
//						for(int tempi = 0; tempi < elementinliveClientTableClient;tempi++){
//							if(strcmp(liveClientTableClient[tempi].IPToShow,argv[1]) == 0){
//								flagTemo = 1;
//								int blockerV2 = 0;
//								int blockedV2 = 0;
//								getIP();
//								blockedV2 = checkBlockListIndex(argv[1]);
//								blockerV2 = checkBlockListIndex(localIpforBlock);
//								if(blocklist[blockerV2][blockedV2] == 0){
//									cse4589_print_and_log("[%s:SUCCESS]\n","BLOCK");
//									printf("%s%s\n","Client received the command : ",command);
//									send(sockfdClient, command, strlen(command),0);
//									cse4589_print_and_log("[%s:END]\n","BLOCK");
//									break;
//								} else{
//									cse4589_print_and_log("[%s:ERROR]\n","BLOCK");
//									cse4589_print_and_log("[%s:END]\n","BLOCK");
//								}
//							}
//						}
//						if(flagTemo == 0){
//							cse4589_print_and_log("[%s:ERROR]\n","BLOCK");
//							cse4589_print_and_log("[%s:END]\n","BLOCK");
//						}
//						cse4589_print_and_log("[%s:SUCCESS]\n","BLOCK");
//						printf("%s%s\n","Client received the command : ",command);
//						send(sockfdClient, command, strlen(command),0);
//						cse4589_print_and_log("[%s:END]\n","BLOCK");
					} else if(strcmp(argv[0],"UNBLOCK") == 0){
						cse4589_print_and_log("[%s:SUCCESS]\n","UNBLOCK");
						printf("%s%s\n","Client received the command : ",command);
						send(sockfdClient, command, strlen(command),0);
						cse4589_print_and_log("[%s:END]\n","UNBLOCK");

					}else if(strcmp(argv[0],"LIST") == 0){
						////TO DO
						/////warning! warning! warning! Just display the currently stored list
						//sort_item(liveClientTable,liveElementInTheTable);
						cse4589_print_and_log("[%s:SUCCESS]\n",command);
						for(int ListPrintIndex =0; ListPrintIndex < elementinliveClientTableClient; ListPrintIndex++){
							//printf("%d %s %s %d\n", ListPrintIndex+1, liveClientTableClient[ListPrintIndex].hostNameToShow,
								   //liveClientTableClient[ListPrintIndex].IPToShow, liveClientTableClient[ListPrintIndex].portNoToShow);
							cse4589_print_and_log("%-5d%-35s%-20s%-8d\n",ListPrintIndex+1,liveClientTableClient[ListPrintIndex].hostNameToShow,
												  liveClientTableClient[ListPrintIndex].IPToShow,liveClientTableClient[ListPrintIndex].portNoToShow);
						}
						cse4589_print_and_log("[%s:END]\n",command);
					}else if(strcmp(argv[0],"REFRESH") == 0){
						////Should receive the latest list (server send the list and send to client, client receive it)
						//// New Added
						cse4589_print_and_log("[%s:SUCCESS]\n","REFRESH");
						if(send(sockfdClient, command, strlen(command),0) == -1){
							perror("send");
						}
						char ListbufFromServer1[1024];
						recv(sockfdClient,ListbufFromServer1, sizeof(ListbufFromServer1),0);
						int argc_newline = 0;
						char *arg_newline, argv_newline[20][50];

						arg_newline = strtok(ListbufFromServer1,"\n");
						while(arg_newline){
							strcpy(argv_newline[argc_newline++],arg_newline);
							arg_newline = strtok(NULL,"\n");
						}
						for(int x = 0; x < argc_newline; x++){
							int argc_space = 0;
							char *arg_space, argv_space[20][50];
							arg_space = strtok(argv_newline[x]," ");
							while(arg_space){
								strcpy(argv_space[argc_space++],arg_space);
								arg_space = strtok(NULL," ");
							}
							strcpy(liveClientTableClient[x].hostNameToShow,argv_space[0]);
							strcpy(liveClientTableClient[x].IPToShow,argv_space[1]);
							liveClientTableClient[x].portNoToShow = atoi(argv_space[2]);
						}
						elementinliveClientTableClient = argc_newline;
						////End of new Added
						cse4589_print_and_log("[%s:END]\n","REFRESH");
					}else if(strcmp(argv[0],"LOGOUT") == 0){
						cse4589_print_and_log("[%s:SUCCESS]\n","LOGOUT");
						send(sockfdClient, command, strlen(command),0);
						close(sockfdClient); //Client will close the socket, the server will update the list
						FD_CLR(sockfdClient,&command_fds);
						cse4589_print_and_log("[%s:END]\n","LOGOUT");
						break;
					}else if(strcmp(argv[0],"EXIT") == 0){
						cse4589_print_and_log("[%s:SUCCESS]\n","EXIT");
						send(sockfdClient, command, strlen(command),0);
						close(sockfdClient); //Client will close the socket, the server will update the list
						FD_CLR(sockfdClient,&command_fds);
						cse4589_print_and_log("[%s:END]\n","EXIT");
						return;
					}
				} else if(i == sockfdClient && i != STDIN_FILENO){
					printf("%s %d %s %d %s %d\n", "i = ",i,"sockfd = ", sockfdClient,"STDIN_FILENO = ", STDIN_FILENO);
					int numbytesClient;
					char bufSendFromServer[500]; //256
					memset(bufSendFromServer,'\0',500); //256

					if((numbytesClient = recv(sockfdClient,bufSendFromServer, sizeof(bufSendFromServer),0)) <= 0){
						if(numbytesClient == 0){
							printf("Server hang up. \n");
						} else{
							perror("recv");
						}
						close(sockfdClient);
						FD_CLR(sockfdClient,&command_fds);
					} else {
						///
						printf("%s\n",bufSendFromServer);
						char recvSendFromIp[50];
						char recvSendToIp[50];
						char recvMessage[300];
						memset(recvMessage,'\0',300);
						memset(recvSendFromIp, '\0', 50);
						memset(recvSendToIp, '\0',50);
						int recvargc_newline = 0;
						char *recvarg_newline, recvargv_newline[300][300];

						recvarg_newline = strtok(bufSendFromServer,"\n");
						while(recvarg_newline){
							strcpy(recvargv_newline[recvargc_newline++],recvarg_newline);
							recvarg_newline = strtok(NULL,"\n");
						}
						cse4589_print_and_log("[%s:SUCCESS]\n","RECEIVED");
						for(int x = 0; x < recvargc_newline; x++){
							memset(recvMessage,'\0',300);
							memset(recvSendFromIp, '\0', 50);
							memset(recvSendToIp, '\0',50);
							int recvargc_space = 0;

							char *recvarg_space, recvargv_space[300][300];
							recvarg_space = strtok(recvargv_newline[x]," ");

							while(recvarg_space){
								strcpy(recvargv_space[recvargc_space++],recvarg_space);
								recvarg_space = strtok(NULL," ");
							}

							strcpy(recvSendFromIp,recvargv_space[0]);
							strcpy(recvSendToIp, recvargv_space[1]);

							for(int spaceIndex = 2; spaceIndex < recvargc_space; spaceIndex++){
								strcat(recvMessage,recvargv_space[spaceIndex]);
								recvMessage[strlen(recvMessage)] = ' ';
							}
							recvMessage[strlen(recvMessage) - 1] = '\0';
							cse4589_print_and_log("msg from:%s\n[msg]:%s\n",recvSendFromIp,recvMessage);
							printf("%d\n",strlen(recvMessage));
						}
						cse4589_print_and_log("[%s:END]\n","RECEIVED");
					}
				}
			}
		}
	}
	//printf("This is a client.");
}
/***********************************************************************************************************************/
/*************************************************End of Client Part code***********************************************/
/***********************************************************************************************************************/


/***********************************************************************************************************************/
/*********************************************Begin of Server Part code*************************************************/
/***********************************************************************************************************************/
void server(){//This Part of teh code will set up the current host as an server, waiting for connection

	fd_set master; //master file descriptor list
	fd_set read_fds;  //temp file descriptor list for select()
	int fdmax; //Maximum file descriptor number

	int elementsInTheTable = 0;  //save the client that has been connected, login or logout for statics
	int liveElementInTheTable = 0; //only login clients
	struct clientToShow clientTable[5]; //create a table that save the connect client information
	struct clientToShow liveClientTable[5]; //create a table that save the current log-in client information, only login clients

	//int blocklist[5][5];

	for(int ii = 0; ii < 5; ii++){
		for(int jj = 0; jj < 5; jj++){
			blocklist[ii][jj] = 0; // 0 indicates unblock
		}
	}

	int listener;
	int newfd;
	struct sockaddr_storage remoteaddr;
	struct addrinfo server_hints, *server_res, *server_p;
	socklen_t addrlen;
	char remoteIP[INET6_ADDRSTRLEN];

	int yes = 1;
	int rv;

	//char buf[256];
	int nbytes;

	//Below is the variable that will store the command from the client. Accroding to what the client send, the server cna respond accrodingly
	int argc=0;
	//char *argfromClient,argvfromClient[20][20];
	char *argfromClient,argvfromClient[20][300];
	char commandfromClient[300];
	char tempcommandfromClient[300];

	///New added code for relay function
	/* 0 - Stones, 1 - embankment, 2 - highgate, 3 - euston, 4 - timberlake*/
	char relayBufferStones[25600];
	memset(relayBufferStones,'\0',25600);
	char relayBufferEmbankment[25600];
	memset(relayBufferEmbankment,'\0',25600);
	char relayBufferHighgate[25600];
	memset(relayBufferHighgate,'\0',25600);
	char relayBufferEuston[25600];
	memset(relayBufferEuston,'\0',25600);
	char relayBufferTimberlake[25600];
	memset(relayBufferTimberlake,'\0',25600);


	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	memset(&server_hints, 0, sizeof server_hints);
	server_hints.ai_family = AF_UNSPEC;
	server_hints.ai_socktype = SOCK_STREAM;
	server_hints.ai_flags = AI_PASSIVE;

	if((rv = getaddrinfo(NULL,port,&server_hints,&server_res)) != 0){
		fprintf(stderr,"getaddrinof: %s\n", gai_strerror(rv));
		return;
	}

	for(server_p = server_res; server_p != NULL; server_p = server_p -> ai_next){
		listener = socket(server_p -> ai_family, server_p -> ai_socktype, server_p -> ai_protocol);
		if(listener < 0){
			continue;
		}
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if(bind(listener, server_p -> ai_addr,server_p -> ai_addrlen) < 0){
			close(listener);
			continue;
		}
		break;
	}

	if (server_p == NULL) {
		fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
	}

	freeaddrinfo(server_res); // all done with this

	if (listen(listener, 10) == -1) {
		perror("listen");
		exit(3);
	}

	FD_SET(listener, &master);
	FD_SET(STDIN_FILENO, &master);

	if(listener > STDIN_FILENO){
		fdmax = listener;
	}else{
		fdmax = STDIN_FILENO;
	}


	for(;;) {
		memcpy(&read_fds, &master, sizeof(master));

		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(4);
		}

		// run through the existing connections looking for data to read
		for(int i = 0; i <= fdmax; i++) {
			if(FD_ISSET(i,&read_fds)){
				if(i == STDIN_FILENO){
					char command[256];
					fgets(command,256,stdin);
					command[strlen(command)-1] = '\0';

					///3/7 new added code
					///3/7 BLOCKED COMMAND will take one argument from the input as the client-ip
					int argc=0;
					char *arg,argv[20][300];

					char temp_command[300]="";
					strcpy(temp_command,command);
					argc = 0;
					arg = strtok(temp_command," ");
					while(arg){
						strcpy(argv[argc++], arg);
						arg = strtok(NULL, " ");
					}
					///
					if(strcmp(command,"AUTHOR") == 0){
						cse4589_print_and_log("[%s:SUCCESS]\n",command);
						commandAUTHOR();
						cse4589_print_and_log("[%s:END]\n",command);
					} else if(strcmp(command,"IP") == 0){
						cse4589_print_and_log("[%s:SUCCESS]\n",command);
						commandIP();
						cse4589_print_and_log("[%s:END]\n",command);
					} else if(strcmp(command,"PORT") == 0){
						cse4589_print_and_log("[%s:SUCCESS]\n",command);
						commandPORT();
						cse4589_print_and_log("[%s:END]\n",command);
					} else if(strcmp(command,"LIST") == 0){
						//just print out the liveClientList table
						sort_item(liveClientTable,liveElementInTheTable);

						cse4589_print_and_log("[%s:SUCCESS]\n",command);
						for(int ListPrintIndex =0; ListPrintIndex < liveElementInTheTable; ListPrintIndex++){
							cse4589_print_and_log("%-5d%-35s%-20s%-8d\n",ListPrintIndex+1, liveClientTable[ListPrintIndex].hostNameToShow,
								   liveClientTable[ListPrintIndex].IPToShow, liveClientTable[ListPrintIndex].portNoToShow,
								   liveClientTable[ListPrintIndex].assignedSocket);
						}
						cse4589_print_and_log("[%s:END]\n",command);
					} else if(strcmp(command,"STATISTICS") == 0){
						//just print out the ClientList table
						sort_item(clientTable,elementsInTheTable);
						cse4589_print_and_log("[%s:SUCCESS]\n",command);
						for(int ListPrintIndex =0; ListPrintIndex < elementsInTheTable; ListPrintIndex++) {
							if (clientTable[ListPrintIndex].status == 1) {
								cse4589_print_and_log("%-5d%-35s%-8d%-8d%-8s\n", ListPrintIndex + 1, clientTable[ListPrintIndex].hostNameToShow,
									   clientTable[ListPrintIndex].numberofMessageSend, clientTable[ListPrintIndex].numberofMessageRec,
									   "logged-in");
							} else if (clientTable[ListPrintIndex].status == 0) {
								cse4589_print_and_log("%-5d%-35s%-8d%-8d%-8s\n", ListPrintIndex + 1, clientTable[ListPrintIndex].hostNameToShow,
									   clientTable[ListPrintIndex].numberofMessageSend, clientTable[ListPrintIndex].numberofMessageRec,
									   "logged-out");
							}
						}
						cse4589_print_and_log("[%s:END]\n",command);
					} else if(strcmp(argv[0],"BLOCKED") == 0){
						char IPaddressfromBLOCKEDcommandS[256];
						memset(IPaddressfromBLOCKEDcommandS,'\0',256);
						strcpy(IPaddressfromBLOCKEDcommandS, argv[1]);
						int boolFlag = 0;
						for(int tempi = 0; tempi < elementsInTheTable; tempi++){
							if(strcmp(clientTable[tempi].IPToShow,IPaddressfromBLOCKEDcommandS) == 0){
								boolFlag = 1;
								break;
							}
						}
						if(boolFlag == 0){
							cse4589_print_and_log("[%s:ERROR]\n","BLOCKED");
							cse4589_print_and_log("[%s:END]\n","BLOCKED");
						} else {
//							char IPaddressfromBLOCKEDcommandS[256];
//							memset(IPaddressfromBLOCKEDcommandS,'\0',256);
//							strcpy(IPaddressfromBLOCKEDcommandS, argv[1]);
							int newIndexforBlockList = 0;
							newIndexforBlockList = checkBlockListIndex(IPaddressfromBLOCKEDcommandS);
							///HANDEL EXCEPTION
							if(newIndexforBlockList == 5){
								cse4589_print_and_log("[%s:ERROR]\n","BLOCKED");
								cse4589_print_and_log("[%s:END]\n","BLOCKED");
								printf("Invalid IP address.\n");
							} else{
								cse4589_print_and_log("[%s:SUCCESS]\n","BLOCKED");
								readBlockList(newIndexforBlockList, liveClientTable);
								cse4589_print_and_log("[%s:END]\n","BLOCKED");
							}
						}
//						cse4589_print_and_log("[%s:SUCCESS]\n","BLOCKED");
//						char IPaddressfromBLOCKEDcommandS[256];
//						memset(IPaddressfromBLOCKEDcommandS,'\0',256);
//						strcpy(IPaddressfromBLOCKEDcommandS, argv[1]);
//						int newIndexforBlockList = 0;
//						newIndexforBlockList = checkBlockListIndex(IPaddressfromBLOCKEDcommandS);
//						///HANDEL EXCEPTION
//						if(newIndexforBlockList == 5){
//							printf("Invalid IP address.\n");
//						} else{
//							readBlockList(newIndexforBlockList, liveClientTable);
//						}
//						cse4589_print_and_log("[%s:END]\n","BLOCKED");
					}

					//fflush(stdout);
				}else if (i == listener) {
					// handle new connections
					addrlen = sizeof remoteaddr;
					newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
					if (newfd == -1) {
						perror("accept");
					} else {
						///
						char portFromClient[10];
						recv(newfd,portFromClient, sizeof(portFromClient),0);
						delay(100);
						///
						FD_SET(newfd, &master); // add to master set
						if (newfd > fdmax) {    // keep track of the max
							fdmax = newfd;
						}
						printf("selectserver: new connection from %s on socket %d\n", inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN), newfd);

						struct clientToShow newClient;
						socklen_t lenConnectClient;
						struct sockaddr_storage addrnewConnectCllient;
						char ipstrnewClient[INET6_ADDRSTRLEN];
						int portNewClient;
						portNewClient = atoi(portFromClient);
						char hostNewClient[1024];
						memset(hostNewClient,'\0',1024);
						char serviceClient[20];
						memset(serviceClient,'\0',20);

						lenConnectClient = sizeof addrnewConnectCllient;
						getpeername(newfd,(struct sockaddr*)&addrnewConnectCllient, &lenConnectClient);
						if(addrnewConnectCllient.ss_family == AF_INET){
							struct sockaddr_in *s = (struct sockaddr_in *)&addrnewConnectCllient;
							inet_ntop(AF_INET,&s->sin_addr,ipstrnewClient,sizeof ipstrnewClient);
						} else {
							struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addrnewConnectCllient;
							inet_ntop(AF_INET6,&s->sin6_addr,ipstrnewClient,sizeof ipstrnewClient);
						}

						printf("Peer IP address: %s\n", ipstrnewClient);
						printf("Peer port : %s\n",portFromClient);

						newClient.assignedSocket = newfd;
						printf("%d\n",newfd);
						strcpy(newClient.IPToShow,ipstrnewClient);
						newClient.portNoToShow = portNewClient;
						newClient.status = 1;
						newClient.numberofMessageRec = 0;
						newClient.numberofMessageSend = 0;

						if(strcmp(newClient.IPToShow,"128.205.36.36") == 0){
							strcpy(newClient.hostNameToShow,"underground.cse.buffalo.edu");
						} else if(strcmp(newClient.IPToShow,"128.205.36.46") == 0){
							strcpy(newClient.hostNameToShow,"stones.cse.buffalo.edu");
						} else if(strcmp(newClient.IPToShow,"128.205.36.35") ==0 ){
							strcpy(newClient.hostNameToShow,"embankment.cse.buffalo.edu");
						} else if(strcmp(newClient.IPToShow,"128.205.36.33") == 0){
							strcpy(newClient.hostNameToShow,"highgate.cse.buffalo.edu");
						} else if(strcmp(newClient.IPToShow,"128.205.36.34") == 0){
							strcpy(newClient.hostNameToShow,"euston.cse.buffalo.edu");
						}else{
							strcpy(newClient.hostNameToShow,"unknown hostname");
						}
						printf("%s\n",newClient.hostNameToShow);
						///if the newclient is not in the clientTable

						///
						int flagtocheck = 0;
						for(int checkTempIndex = 0; checkTempIndex < elementsInTheTable; checkTempIndex++){
							if(strcmp(newClient.hostNameToShow, clientTable[checkTempIndex].hostNameToShow) == 0){
								clientTable[checkTempIndex].status = 1;
								clientTable[checkTempIndex].assignedSocket = newfd;
								clientTable[checkTempIndex].portNoToShow = portNewClient;
								flagtocheck = 1;
								break;
							}
						}

						if(flagtocheck == 0){
							add_item(clientTable,newClient,elementsInTheTable);
							elementsInTheTable++;
						}

						add_item(liveClientTable,newClient,liveElementInTheTable);
						liveElementInTheTable++;
						sort_item(clientTable,elementsInTheTable);
						sort_item(liveClientTable,liveElementInTheTable);

						///// This is actually received the login command from client, after the server accept the login client, it should send the client
						//// current login client list

						char ListbufSendToNewClient[1024];
						memset(ListbufSendToNewClient,'\0',1024);
						for(int f = 0; f < liveElementInTheTable; f++){
							strcat(ListbufSendToNewClient, liveClientTable[f].hostNameToShow);
							ListbufSendToNewClient[strlen(ListbufSendToNewClient)] = ' ';
							strcat(ListbufSendToNewClient,liveClientTable[f].IPToShow);
							ListbufSendToNewClient[strlen(ListbufSendToNewClient)] = ' ';
							char portNumberChar[1024];
							//memset(portNewClient,'\0',50);
							sprintf(portNumberChar,"%d",liveClientTable[f].portNoToShow);
							strcat(ListbufSendToNewClient,portNumberChar);
							printf("%d %s\n",f,portNumberChar);
							ListbufSendToNewClient[strlen(ListbufSendToNewClient)] = '\n';
						}
						send(newfd,ListbufSendToNewClient,sizeof ListbufSendToNewClient,0);

						/// 3/8/2017 New added:
						///Handle the buffer data of the login client
						char requestBuffer[50];
						strcpy(requestBuffer,"Y");
						char requestBuffer2[50];
						strcpy(requestBuffer2,"N");
						delay(100);
						if(strcmp(newClient.IPToShow, "128.205.36.46") == 0){
							printf("Check stored buffer for Stones. \n");
							if(strlen(relayBufferStones) != 0){
								send(newfd,requestBuffer,sizeof requestBuffer,0);
								//delay(100);
								readRelayBuffer(relayBufferStones);
								delay(100);
								send(newfd, relayBufferStones,sizeof relayBufferStones,0);
								memset(relayBufferStones,'\0',25600);
							} else{
								printf("Nothing to send.\n");
								delay(100);
								send(newfd,requestBuffer2,sizeof requestBuffer2,0);
							}
						} else if(strcmp(newClient.IPToShow, "128.205.36.35") == 0){
							printf("Check stored buffer for Embank. \n");
							if(strlen(relayBufferEmbankment) != 0){
								send(newfd,requestBuffer,sizeof requestBuffer,0);
								//delay(100);
								readRelayBuffer(relayBufferEmbankment);
								delay(100);
								send(newfd, relayBufferEmbankment,sizeof relayBufferEmbankment,0);
								memset(relayBufferEmbankment,'\0',25600);
							} else {
								printf("Nothing to send.\n");
								delay(100);
								send(newfd,requestBuffer2,sizeof requestBuffer2,0);
							}
						} else if(strcmp(newClient.IPToShow,"128.205.36.33") == 0){
							//Relay the message to char relayBufferHighgate[5][2560];
							if(strlen(relayBufferHighgate) != 0){
								send(newfd,requestBuffer,sizeof requestBuffer,0);
								readRelayBuffer(relayBufferHighgate);
								delay(100);
								send(newfd, relayBufferHighgate,sizeof relayBufferHighgate,0);
								memset(relayBufferHighgate,'\0',25600);
							} else{
								printf("Nothing to send.\n");
								delay(100);
								send(newfd,requestBuffer2,sizeof requestBuffer2,0);
							}
						} else if(strcmp(newClient.IPToShow,"128.205.36.34") == 0){
							//Relay the message to char relayBufferEuston[5][2560];
							if(strlen(relayBufferEuston) != 0){
								send(newfd,requestBuffer,sizeof requestBuffer,0);
								readRelayBuffer(relayBufferEuston);
								delay(100);
								send(newfd, relayBufferEuston,sizeof relayBufferEuston,0);
								memset(relayBufferEuston,'\0',25600);
							} else{
								printf("Nothing to send.\n");
								delay(100);
								send(newfd,requestBuffer2,sizeof requestBuffer2,0);
							}
						} else if(strcmp(newClient.IPToShow,"128.205.36.36") == 0){
							//Relay the message to char relayBufferTimberlake[5][2560];
							if(strlen(relayBufferTimberlake) != 0){
								send(newfd,requestBuffer,sizeof requestBuffer,0);
								readRelayBuffer(relayBufferTimberlake);
								delay(100);
								send(newfd, relayBufferTimberlake,sizeof relayBufferTimberlake,0);
								memset(relayBufferTimberlake,'\0',25600);
							} else{
								printf("Nothing to send.\n");
								delay(100);
								send(newfd,requestBuffer2,sizeof requestBuffer2,0);
							}
						}
						///End Handle the buffer data of the login client
					}
				} else {
					///handle data from a client
					char buf[512];
					memset(buf, '\0', 512);
					if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
						if (nbytes == 0) {
							printf("selectserver: socket %d hung up\n", i);
						} else {
							perror("recv");
						}
						close(i); // bye!
						FD_CLR(i, &master); // remove from master set
					} else {
						// we got some data from a client
						printf("\nClient sent me: %s\n", buf);
						strcpy(commandfromClient,buf);
						strcpy(tempcommandfromClient,buf); //copy the command from the client and store it in tempcommandfromClient.
						argc = 0;
						argfromClient = strtok(tempcommandfromClient," ");
						while(argfromClient){
							strcpy(argvfromClient[argc++], argfromClient);
							argfromClient = strtok(NULL, " ");
						}
						printf("%s %s\n","The first word is :",argvfromClient[0]);
						if(strcmp(argvfromClient[0],"SEND") == 0){
							char sendfromIP[50];
							memset(sendfromIP, '\0', 50);
							char sendtoIP[50];
							memset(sendfromIP,'\0',50);
							strcpy(sendtoIP, argvfromClient[1]);
							for(int tempi = 0; tempi<5; tempi++){
								if(i == liveClientTable[tempi].assignedSocket){
									strcpy(sendfromIP, liveClientTable[tempi].IPToShow);
									break;
								}
							}
							char restBufRelay[500];
							memset(restBufRelay,'\0',500);
							for(int tempi = 2; tempi < argc; tempi++){
								strcat(restBufRelay,argvfromClient[tempi]);
								restBufRelay[strlen(restBufRelay)]=' ';
							}
//							cse4589_print_and_log("[%s:SUCCESS]\n","RELAYED");
//							cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n",sendfromIP,sendtoIP,restBufRelay);
//							cse4589_print_and_log("[%s:END]\n","RELAYED");

							///
							for(int tempi = 0; tempi < 5;tempi++){
								if(i == clientTable[tempi].assignedSocket){
									clientTable[tempi].numberofMessageSend++;
									break;
								}
							}
							///
							//Do the send command, argvfromClient[1] is ip address, argvfromClient[2] is the message content.
							for(int qq = 0; qq < elementsInTheTable; qq++){
								if(strcmp(clientTable[qq].IPToShow, argvfromClient[1]) == 0){ //The client doesn't exit
									clientTable[qq].numberofMessageRec++;
									int sendfromid = 0;
									int sendtoid = 0;
									sendfromid = checkSocketTogetIndex(clientTable,elementsInTheTable,i);
									sendtoid = checkBlockListIndex(argvfromClient[1]);
									printf("%s %d\n","The integer in block list is : ",blocklist[sendtoid][sendfromid]);
									if(blocklist[sendtoid][sendfromid] == 1){
										printf("The receiver side client has blocked this send side client. \n");
										printf("I will BLOCK the message for you! \n");
										break;
									} else if(blocklist[sendtoid][sendfromid] == 0){
										printf("%s\n", argvfromClient[1]);
										///
										char restBuf2[500]; //300
										memset(restBuf2,'\0',500);
										strcat(restBuf2,sendfromIP);
										restBuf2[strlen(restBuf2)]=' ';
										strcat(restBuf2,sendtoIP);
										restBuf2[strlen(restBuf2)]=' ';
										for(int tempi = 2; tempi < argc; tempi++){
											strcat(restBuf2,argvfromClient[tempi]);
											restBuf2[strlen(restBuf2)]=' ';
										}
										//restBuf2[strlen(restBuf2)-1]='\n'; //'\0'
										restBuf2[strlen(restBuf2)]='\n'; //////
										///
										if(clientTable[qq].status == 0){ //The client is off line, but no exit, need to store message for it
											printf("Server : The client is offline, I will store the message for this client.\n");
											char IPfromClientTable[50];
											memset(IPfromClientTable, '\0',50);
											strcpy(IPfromClientTable, clientTable[qq].IPToShow);
											if(strcmp(IPfromClientTable, "128.205.36.46") == 0){
												//stones
												printf("Store message for Stones.\n");
												strcat(relayBufferStones, restBuf2);
												printf("%s\n",relayBufferStones);
											} else if(strcmp(IPfromClientTable, "128.205.36.35") == 0){
												//Relay the message to char relayBufferEmbankment[5][2560];
												strcat(relayBufferEmbankment, restBuf2);
												printf("%s\n",relayBufferEmbankment);
											} else if(strcmp(IPfromClientTable,"128.205.36.33") == 0){
												//Relay the message to char relayBufferHighgate[5][2560];
												strcat(relayBufferHighgate, restBuf2);
												printf("%s\n",relayBufferHighgate);
											} else if(strcmp(IPfromClientTable,"128.205.36.33") == 0){
												//Relay the message to char relayBufferEuston[5][2560];
												strcat(relayBufferEuston, restBuf2);
												printf("%s\n",relayBufferEuston);
											} else if(strcmp(IPfromClientTable,"128.205.36.36") == 0){
												//Relay the message to char relayBufferTimberlake[5][2560];
												strcat(relayBufferTimberlake, restBuf2);
												printf("%s\n",relayBufferTimberlake);
											}
											break;
										} else if(clientTable[qq].status == 1) {//The client is online send the message
											//printf("Send the message to destination.\n");
											cse4589_print_and_log("[%s:SUCCESS]\n","RELAYED");
											cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n",sendfromIP,sendtoIP,restBufRelay);
											cse4589_print_and_log("[%s:END]\n","RELAYED");
											if(send(clientTable[qq].assignedSocket, restBuf2, strlen(restBuf2),0) <= 0){
												perror("send");
											}
											break;
										}
									}
								}
							}
						} else if(strcmp(argvfromClient[0],"BROADCAST") == 0){
							///Broadcast the message, should still consider the Block situation
							///Need to update send and receive for the Statistics command
							char sendfromIP[50];
							memset(sendfromIP, '\0', 50);
							char sendtoIP[50];
							memset(sendfromIP,'\0',50);
							strcpy(sendtoIP, "255.255.255.255");
							for(int tempi = 0; tempi<5; tempi++){
								if(i == liveClientTable[tempi].assignedSocket){
									strcpy(sendfromIP, liveClientTable[tempi].IPToShow);
									break;
								}
							}
							///For now, the broadcast only increase 1 for the sender
							for(int tempi = 0; tempi<5; tempi++){
								if(i == clientTable[tempi].assignedSocket){
									clientTable[tempi].numberofMessageSend++;
									//clientTable[tempi].numberofMessageSend = clientTable[tempi].numberofMessageSend + elementsInTheTable;
									break;
								}
							}
							///
							char restBuf[500];
							char restBufMsg[500];
							memset(restBuf,'\0',500);
							memset(restBufMsg,'\0',500);
							strcat(restBuf,sendfromIP);
							restBuf[strlen(restBuf)]=' ';
							strcat(restBuf,sendtoIP);
							restBuf[strlen(restBuf)]=' ';
							for(int tempi = 1; tempi < argc; tempi++){
								strcat(restBuf,argvfromClient[tempi]);
								strcat(restBufMsg,argvfromClient[tempi]);
								restBuf[strlen(restBuf)]=' ';
								restBufMsg[strlen(restBufMsg)]=' ';
							}
							//restBuf[strlen(restBuf)-1]='\n'; //'\0'
							//restBuf[strlen(restBuf)-1]='\0';
							cse4589_print_and_log("[%s:SUCCESS]\n","RELAYED");
							cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n",sendfromIP,sendtoIP,restBufMsg);
							cse4589_print_and_log("[%s:END]\n","RELAYED");
							//restBuf[strlen(restBuf)-1]='\n';
							restBuf[strlen(restBuf)]='\n';
							for(int j = 0; j <= fdmax; j++) {
								if (FD_ISSET(j, &master)) {//The client is in the live client table
									///Check Block Status
									int blocker = 0;
									int blocked = 0;
									blocker = checkSocketTogetIndex(liveClientTable,liveElementInTheTable,j);
									blocked = checkSocketTogetIndex(liveClientTable,liveElementInTheTable,i);
									if(blocklist[blocker][blocked] != 1){ //does not block
										if (j != listener && j != i && j != STDIN_FILENO) {// except the listener and ourselves
											printf("Send the broadcast message to client.\n");
											printf("%s%s\n", "The buf is : ", restBuf);
											if(send(j, restBuf, strlen(restBuf),0) <= 0){
												perror("send");
											} else{
												for(int tempi = 0; tempi < 5; tempi++){
													if(j == clientTable[tempi].assignedSocket){
														clientTable[tempi].numberofMessageRec++;
														break;
													}
												}
											}
										}
									} else if(blocklist[blocker][blocked] == 1){
										printf("Blocked, will not receive broadcast message from this sender.\n");
									}
								} else{//The client is not in the live client table, check client table
									for(int tempInedxv = 0; tempInedxv < 5; tempInedxv++){
										if(clientTable[tempInedxv].assignedSocket == j){//The client with this socket number is logout
											printf("I will store the message for you.\n");
											///
											clientTable[tempInedxv].numberofMessageRec++;
											///
											///
											char IPfromClientTable[50];
											memset(IPfromClientTable, '\0',50);
											strcpy(IPfromClientTable, clientTable[tempInedxv].IPToShow);
											if(strcmp(IPfromClientTable, "128.205.36.46") == 0){
												//stones
												printf("Store message for Stones.\n");
												strcat(relayBufferStones, restBuf);
												printf("%s\n",relayBufferStones);
											} else if(strcmp(IPfromClientTable, "128.205.36.35") == 0){
												//Relay the message to char relayBufferEmbankment[5][2560];
												strcat(relayBufferEmbankment, restBuf);
												printf("%s\n",relayBufferEmbankment);
											} else if(strcmp(IPfromClientTable,"128.205.36.33") == 0){
												//Relay the message to char relayBufferHighgate[5][2560];
												strcat(relayBufferHighgate, restBuf);
												printf("%s\n",relayBufferHighgate);
											} else if(strcmp(IPfromClientTable,"128.205.36.33") == 0){
												//Relay the message to char relayBufferEuston[5][2560];
												strcat(relayBufferEuston, restBuf);
												printf("%s\n",relayBufferEuston);
											} else if(strcmp(IPfromClientTable,"128.205.36.36") == 0){
												//Relay the message to char relayBufferTimberlake[5][2560];
												strcat(relayBufferTimberlake, restBuf);
												printf("%s\n",relayBufferTimberlake);
											}
											///
											break;
										}
									}
								}
							}
						} else if(strcmp(argvfromClient[0],"BLOCK") == 0){
							//BLOCK the client with argvfromClient[1] ip address
							////Get the IP address of the sender
							printf("I am going to block this. \n");
							char beBlockedIPaddress[50];
							memset(beBlockedIPaddress,'\0',50);
							strcpy(beBlockedIPaddress, argvfromClient[1]);
							int flagII = 0;
							for(int tempii = 0; tempii < liveElementInTheTable; tempii++){
								if(strcmp(liveClientTable[tempii].IPToShow,beBlockedIPaddress) == 0){
									flagII = 1;
									break;
								}
							}
							if(flagII == 0){
								cse4589_print_and_log("[%s:ERROR]\n","BLOCK");
								cse4589_print_and_log("[%s:END]\n","BLOCK");
							} else {
								int blocker = 0;
								int blocked = 0;
								blocker = checkSocketTogetIndex(liveClientTable, liveElementInTheTable, i);
								blocked = checkBlockListIndex(beBlockedIPaddress);
								if (blocker == 5 || blocked == 5) {
									cse4589_print_and_log("[%s:ERROR]\n", "BLOCK");
									cse4589_print_and_log("[%s:END]\n", "BLOCK");
								} else {
									if (blocklist[blocker][blocked] == 1) {
										cse4589_print_and_log("[%s:ERROR]\n", "BLOCK");
										cse4589_print_and_log("[%s:END]\n", "BLOCK");
									} else {
										cse4589_print_and_log("[%s:SUCCESS]\n","BLOCK");
										updateBlockList(blocker,blocked);
										cse4589_print_and_log("[%s:END]\n","BLOCK");
									}
								}
							}
//								if(blocklist[blocker][blocked] == 1){
//									cse4589_print_and_log("[%s:ERROR]\n","BLOCK");
//									cse4589_print_and_log("[%s:END]\n","BLOCK");
//								}else{
//									if((blocker != 5 )&& (blocked != 5)){
//										cse4589_print_and_log("[%s:SUCCESS]\n","BLOCK");
//										updateBlockList(blocker,blocked);
//										cse4589_print_and_log("[%s:END]\n","BLOCK");
//										//printf(blocklist)
//									} else {
//										cse4589_print_and_log("[%s:ERROR]\n","BLOCK");
//										cse4589_print_and_log("[%s:END]\n","BLOCK");
//										printf("Exception need to be handle. \n");
//									}
//								}
//							}
							//Now get two index one from the IP address, one from the socket
//							int blocker = 0;
//							int blocked = 0;
//							blocker = checkSocketTogetIndex(liveClientTable,liveElementInTheTable,i);
//							blocked = checkBlockListIndex(beBlockedIPaddress);
//							if((blocker != 5 )&& (blocked != 5)){
//								updateBlockList(blocker,blocked);
//								//printf(blocklist)
//							} else {
//								cse4589_print_and_log("[%s:ERROR]\n","BLOCK");
//								cse4589_print_and_log("[%s:END]\n","BLOCK");
//								printf("Exception need to be handle. \n");
//							}
						} else if(strcmp(argvfromClient[0],"UNBLOCK") == 0){
							//Unblock the client with argvfromClient[1] ip address
							printf("I am going to block this. \n");
							char beBlockedIPaddress[50];
							memset(beBlockedIPaddress,'\0',50);
							strcpy(beBlockedIPaddress, argvfromClient[1]);
							//Now get two index one from the IP address, one from the socket
							int blocker = 0;
							int blocked = 0;
							blocker = checkSocketTogetIndex(liveClientTable,liveElementInTheTable,i);
							blocked = checkBlockListIndex(beBlockedIPaddress);
							if((blocker != 5 )&& (blocked != 5)){
								unblockFromBlockList(blocker,blocked);
							} else {
								printf("Exception need to be handle. \n");
							}
						} else if(strcmp(commandfromClient,"LOGOUT") == 0){
							//Be aware this client has been logout.
							close(i);
							FD_CLR(i,&master);
							for(int temp = 0; temp < liveElementInTheTable; temp++){
								if(i == liveClientTable[temp].assignedSocket){
									delete_item(liveClientTable,liveElementInTheTable,temp);
									liveElementInTheTable--;
									printf("Delete from the LiveClient Table.\n");
								}
							}
							for(int temp = 0; temp < elementsInTheTable; temp++){
								if(i == clientTable[temp].assignedSocket){
									clientTable[temp].status = 0;
									printf("Change the status from Client Table to LogOut.\n");
								}
							}
							////New Added Sort the list by port number
							sort_item(clientTable,elementsInTheTable);
							sort_item(liveClientTable,liveElementInTheTable);
							////End of new Added Sort the list by port number
						}else if(strcmp(argvfromClient[0],"LIST") == 0){

						}else if(strcmp(argvfromClient[0],"REFRESH") == 0){
							char ListBufSendToClientFresh[1024];
							memset(ListBufSendToClientFresh, '\0', 1024);
							for(int refreshIndex = 0; refreshIndex < liveElementInTheTable;refreshIndex++){
								strcat(ListBufSendToClientFresh,liveClientTable[refreshIndex].hostNameToShow);
								ListBufSendToClientFresh[strlen(ListBufSendToClientFresh)] = ' ';
								strcat(ListBufSendToClientFresh,liveClientTable[refreshIndex].IPToShow);
								ListBufSendToClientFresh[strlen(ListBufSendToClientFresh)] = ' ';
								char portNumberCharRefresh[50];
								memset(portNumberCharRefresh,'\0',50);
								sprintf(portNumberCharRefresh,"%d",liveClientTable[refreshIndex].portNoToShow);
								strcat(ListBufSendToClientFresh,portNumberCharRefresh);
								ListBufSendToClientFresh[strlen(ListBufSendToClientFresh)] = '\n';
							}
							send(i,ListBufSendToClientFresh,sizeof(ListBufSendToClientFresh),0);

						} else if(strcmp(commandfromClient,"EXIT") == 0){
							//Be aware this client has been EXIT
							////3/10
							close(i);
							FD_CLR(i,&master);
							for(int temp = 0; temp < liveElementInTheTable; temp++){
								if(i == liveClientTable[temp].assignedSocket){
									delete_item(liveClientTable,liveElementInTheTable,temp);
									liveElementInTheTable--;
									printf("Delete from the LiveClient Table.\n");
								}
							}
							for(int temp = 0; temp < elementsInTheTable; temp++){
								if(i == clientTable[temp].assignedSocket){
									//clientTable[temp].status = 0;
									delete_item(clientTable,elementsInTheTable,temp);
									break;
									//printf("Change the status from Client Table to LogOut.\n");
								}
							}
							////
							////New Added : Sort the list by port number
							sort_item(clientTable,elementsInTheTable);
							sort_item(liveClientTable,liveElementInTheTable);
							////End of new Added : Sort the list by port number
						}
						////Broadcast Original Position
					}
				} // END handle data from client
			} // END got new incoming connection
		} // END looping through file descriptors
	}
}
/***********************************************************************************************************************/
/***********************************************End of Server Part code*************************************************/
/***********************************************************************************************************************/

void commandAUTHOR(){
	//printf("I, %s, have read and understand the course academic integrity policy.\n", "yuzeliu");
    //cse4589_print_and_log(“I, %s, have read and understood the course academic integrity policy.\n”, your_ubit_name);
    cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "yuzeliu");
}

void commandIP(){
	struct addrinfo hints, *res, *p;
	char ipstr[INET6_ADDRSTRLEN];
	int status;
	char hostname[256];
	int temp;
	if(temp = gethostname(hostname, sizeof hostname) != 0){
		fprintf(stderr,"get hostname :%s\n", gai_strerror(temp));

	}

	memset(&hints, 0,sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if((status = getaddrinfo(hostname, port, &hints, &res)) != 0){
		//cse4589_print_and_log("[%s:ERROR]\n",command_str);
		//cse4589_print_and_log("[%s:END]\n",command_str);
		fprintf(stderr,"getaddrinfo : %s\n",gai_strerror(status));
		return;
	}

	for(p=res; p!=NULL; p = p -> ai_next){
		void *addr;
		char *ipver;

		if(p->ai_family == AF_INET){
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)p -> ai_addr;
			addr = &(ipv4 ->sin_addr);
			ipver = "IPv4";
		} else {
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p -> ai_addr;
			addr = &(ipv6 -> sin6_addr);
			ipver = "IPv6";
		}

		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
		//printf("%s:%s\n", ipver, ipstr);
		//cse4589_print_and_log([%s:SUCCESS]\n",command_str);
		//ipstr[strlen(ipstr)]='\0';
		cse4589_print_and_log("IP:%s\n",ipstr);
		//cse4589_print_and_log("[%s:END]\n",command_str);
		strcpy(localIpforBlock,ipstr);
	}
	freeaddrinfo(res);
	return;
}

void commandPORT(){
	//Print the port number this process is listening on.
	//printf("The port number is %s\n", port);
	int portToDisplay;
	portToDisplay = atoi(port);
	cse4589_print_and_log("PORT:%d\n",portToDisplay);

}

int commandLogin(char ipaddress[256], char portNo[256]){
	//connect the client to the server
	int sockfd;
	struct addrinfo hints, *res, *p;
	int status;

	memset(&hints, 0,sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if((status = getaddrinfo(ipaddress,portNo,&hints, &res)) != 0){
		fprintf(stderr,"getaddrinfo : %s\n",gai_strerror(status));
		cse4589_print_and_log("[%s:ERROR]\n","LOGIN");
		cse4589_print_and_log("[%s:END]\n","LOGIN");
		//exit(-1);
		return -1;
	}

	for(p = res; p != NULL; p = p -> ai_next){
		if((sockfd = socket( p -> ai_family, p -> ai_socktype, p -> ai_protocol)) == -1){
			perror("client : socket");
			continue;
		}

		if(connect(sockfd, p -> ai_addr, p -> ai_addrlen) == -1){
			close(sockfd);
			perror("client : connect");
			continue;
		}
		break;
	}

	if(p == NULL){
		fprintf(stderr, "client : failed to connect\n");
		exit(-1);
	}

	freeaddrinfo(res);

	printf("client : connecting to the server successfully.\n");
	printf("%d\n", sockfd);

	return sockfd;
}


void *get_in_addr(struct sockaddr *sa){
	if(sa -> sa_family == AF_INET){
		return &(((struct sockaddr_in*)sa) -> sin_addr);
	}
	return &(((struct sockaddr_in6 *)sa) -> sin6_addr);
}

void add_item(struct clientToShow *p,struct clientToShow a,int num_items)
{
	if ( num_items < 5 )
	{
		p[num_items] = a;
	}
}

void delete_item(struct clientToShow *p,int num_items, int item)
{
	if (num_items > 0 && item < num_items && item > -1)
	{
		int last_index = num_items - 1;
		for (int i = item; i < last_index;i++)
		{
			p[i] = p[i + 1];
		}
	}
}

void swap_item(struct clientToShow *a, struct clientToShow *b){
	struct clientToShow c_temp;

	strcpy(c_temp.hostNameToShow, b->hostNameToShow);
	strcpy(c_temp.IPToShow,b->IPToShow);

	c_temp.portNoToShow = b->portNoToShow;
	c_temp.numberofMessageRec = b->numberofMessageRec;
	c_temp.numberofMessageSend = b->numberofMessageSend;
	c_temp.assignedSocket = b->assignedSocket;
	c_temp.status = b->status;

	//b = a;
	strcpy(b->hostNameToShow, a->hostNameToShow);
	strcpy(b->IPToShow,a->IPToShow);
	b->portNoToShow = a->portNoToShow;
	b->numberofMessageRec = a->numberofMessageRec;
	b->numberofMessageSend = a->numberofMessageSend;
	b->assignedSocket = a->assignedSocket;
	b->status = a->status;

	//a = c_temp
	strcpy(a->hostNameToShow, c_temp.hostNameToShow);
	strcpy(a->IPToShow,c_temp.IPToShow);
	a->portNoToShow = c_temp.portNoToShow;
	a->numberofMessageRec = c_temp.numberofMessageRec;
	a->numberofMessageSend = c_temp.numberofMessageSend;
	a->assignedSocket = c_temp.assignedSocket;
	a->status = c_temp.status;

}

void sort_item(struct clientToShow *p, int count)
{
	for(int i = 0; i < count; i ++){
		int min = p[i].portNoToShow;
		for(int j = i+1; j<count;j++){
			if(min > p[j].portNoToShow){
				min = p[j].portNoToShow;
				swap_item(&p[i],&p[j]);
			}
		}
	}
}

///Use 2-D integer array to store the blocklist for each client. The last element of each row store the number od blocked client of this client.
int checkBlockListIndex(char ipaddress[256]){
	int returnIndex = 0;

	if(strcmp(ipaddress,"128.205.36.46") == 0){
		returnIndex = 0;
	} else if(strcmp(ipaddress,"128.205.36.35") == 0){
		returnIndex = 1;
	} else if(strcmp(ipaddress,"128.205.36.33") == 0){
		returnIndex = 2;
	} else if(strcmp(ipaddress,"128.205.36.34") == 0){
		returnIndex = 3;
	} else if(strcmp(ipaddress,"128.205.36.36") == 0){
		returnIndex = 4;
	} else{
		printf("Invalid IP address from checkBlockListIndex Function. \n");
		returnIndex = 5;
	}
	//printf("CheckBlockListIndex returns : %d\n", returnIndex);
	return  returnIndex;
}

///Update the block list, take 2 arguments, "blocker" blocks "blocked"
void updateBlockList(int blocker, int blocked){
	if(blocklist[blocker][blocked] == 0){
		blocklist[blocker][blocked] = 1;
	} else{
		printf("Already blocked.\n");
	}
}

void unblockFromBlockList(int unblocker, int unblocked){
	if(blocklist[unblocker][unblocked] == 1){
		blocklist[unblocker][unblocked] = 0;
	} else {
		printf("Already unblocked. \n");
	}
}

///Convert the socket to the bloack index, from socket and the liveclient table, get the IP addresss, then call checkBlockedListIndex fuction to get index
int checkSocketTogetIndex(struct clientToShow *p, int count, int socketToCheck){
	int indexGet = 0;
	char ipaddress[256];
	memset(ipaddress,'\0',256);
	for(int i = 0; i < count; i++){
		if(socketToCheck == p[i].assignedSocket){
			strcpy(ipaddress, p[i].IPToShow);
			indexGet = checkBlockListIndex(ipaddress);
			return indexGet;
		}
	}
	indexGet = 5;
	return  indexGet;
}

void readBlockList(int blocker, struct clientToShow *p){
	///Warning! We assumed that the blocked one is online, need to handle exception later.
	printf("%s\n", "I am gonna to print all the blocked client for this bitch client!");
	struct clientToShow blockedtable[5];
	int number = 0;
	char ipaddress[256];

	for(int i = 0; i < 5; i++){
		memset(ipaddress,'\0',256);
		if(blocklist[blocker][i] == 1){
			if(i == 0){
				strcpy(ipaddress,"128.205.36.46");
			}else if(i == 1){
				strcpy(ipaddress,"128.205.36.35");
			}else if(i == 2){
				strcpy(ipaddress,"128.205.36.33");
			}else if(i == 3){
				strcpy(ipaddress,"128.205.36.34");
			}else if(i == 4){
				strcpy(ipaddress,"128.205.36.36");
			}
			strcpy(blockedtable[number].IPToShow,ipaddress);
			for(int j = 0; j<5; j++){
				if(strcmp(ipaddress, p[j].IPToShow) == 0){
					strcpy(blockedtable[number].hostNameToShow,p[j].hostNameToShow);
					blockedtable[number].portNoToShow = p[j].portNoToShow;
					break;
				}
			}
			number++;
		}
	}
	sort_item(blockedtable,number);
	for(int i = 0; i < number; i++){
//		printf("%d %s %s %d\n", i+1, blockedtable[i].hostNameToShow,
//			   blockedtable[i].IPToShow, blockedtable[i].portNoToShow);
		cse4589_print_and_log("%-5d%-35s%-20s%-8d\n",i+1,blockedtable[i].hostNameToShow,
			   blockedtable[i].IPToShow, blockedtable[i].portNoToShow);
	}
}
void delay(int mseconds){
	clock_t goal = mseconds + clock();
	while(goal > clock());
}

void readRelayBuffer(char *relayBuffer){
	char recvSendFromIp[50];
	char recvSendToIp[50];
	char recvMessage[256];
	memset(recvMessage,'\0',256);
	memset(recvSendFromIp, '\0', 50);
	memset(recvSendToIp, '\0',50);
	int recvargc_newline = 0;
	char *recvarg_newline, recvargv_newline[20][300];
	recvarg_newline = strtok(relayBuffer,"\n");
	while(recvarg_newline){
		strcpy(recvargv_newline[recvargc_newline++],recvarg_newline);
		recvarg_newline = strtok(NULL,"\n");
	}
	for(int x = 0; x < recvargc_newline; x++){
		memset(recvMessage,'\0',256);
		memset(recvSendFromIp, '\0', 50);
		memset(recvSendToIp, '\0',50);
		int recvargc_space = 0;
		char *recvarg_space, recvargv_space[20][300];
		recvarg_space = strtok(recvargv_newline[x]," ");
		while(recvarg_space){
			strcpy(recvargv_space[recvargc_space++],recvarg_space);
			recvarg_space = strtok(NULL," ");
		}
		strcpy(recvSendFromIp,recvargv_space[0]);
		strcpy(recvSendToIp, recvargv_space[1]);
		for(int spaceIndex = 2; spaceIndex < recvargc_space; spaceIndex++){
			strcat(recvMessage,recvargv_space[spaceIndex]);
			recvMessage[strlen(recvMessage)] = ' ';
		}
		//recvMessage[strlen(recvMessage) - 1] = '\0';
		cse4589_print_and_log("[%s:SUCCESS]\n","RELAYED");
		cse4589_print_and_log("msg from:%s\n[msg]:%s\n",recvSendFromIp,recvMessage);
		cse4589_print_and_log("[%s:END]\n","RELAYED");
	}
}

void getIP(){
	struct addrinfo hints, *res, *p;
	char ipstr[INET6_ADDRSTRLEN];
	int status;
	char hostname[256];
	int temp;
	if(temp = gethostname(hostname, sizeof hostname) != 0){
		fprintf(stderr,"get hostname :%s\n", gai_strerror(temp));
	}

	memset(&hints, 0,sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if((status = getaddrinfo(hostname, port, &hints, &res)) != 0){
		//cse4589_print_and_log("[%s:ERROR]\n",command_str);
		//cse4589_print_and_log("[%s:END]\n",command_str);
		fprintf(stderr,"getaddrinfo : %s\n",gai_strerror(status));
		return;
	}

	for(p=res; p!=NULL; p = p -> ai_next){
		void *addr;
		char *ipver;

		if(p->ai_family == AF_INET){
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)p -> ai_addr;
			addr = &(ipv4 ->sin_addr);
			ipver = "IPv4";
		} else {
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p -> ai_addr;
			addr = &(ipv6 -> sin6_addr);
			ipver = "IPv6";
		}

		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
		//printf("%s:%s\n", ipver, ipstr);
		//cse4589_print_and_log([%s:SUCCESS]\n",command_str);
		//ipstr[strlen(ipstr)]='\0';
		//cse4589_print_and_log("IP:%s\n",ipstr);
		//cse4589_print_and_log("[%s:END]\n",command_str);
		strcpy(localIpforBlock,ipstr);
	}
	freeaddrinfo(res);
	return;
}
