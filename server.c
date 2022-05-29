#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <mysql.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>

#define PORT 2999
#define MAX_COMMAND_LENG 100
#define MAX_REPLY_LENG 6000
#define V 37

extern int errno;

typedef struct thData{
	int idThread; 
	int cl;
}thData;
typedef struct allSocketDesc
{
    int sd[100];
    int activ[100];
}allSocketDesc;

int indexForPath = 0;
int path[37];
pthread_mutex_t lock; //race conditions

void *handleClient(void *);
void extractArgument(char comanda[], int index, char argument[]);
int verifyNrArguments(char comanda[]);
void removeSpacesAndNewLine(char comanda[]);
int parseNewsletter(char userCommand[], char op1[], char op2[], char op3[]);
int verifyComposeNameStreet(char streetName[]);
void removeSpacesAndNewLineComposedStreet(char streetName[]);
void getNewsletterInfo(char buffer[], MYSQL *traficDataBase, char option[]);
void makePacket(char commandMessage[], int quit, int loggedIn, int lastCommandIsSpeed, int ifAccident, int ifValidStreet, int length, char packet[]);
void errorDB(MYSQL *thisDataBase);
int verifyUsernameDB(MYSQL *thisDataBase, char name[]);
int generateUserId();
int isNumber(char num[]);
int checkIfStreetExists(MYSQL *traficDataBase, char mysqlQuery[]);
void decodePacket(char commandPacket[], char command[], int *commandLength);
int countLetters(char command[]);
void notCaseSensitive(char word[]);
void clearAccidentTrafficJamTable();
int minDistance(int dist[], bool sptSet[]);
void findPath(int parent[], int j);
void dijkstra(int graph[V][V], int src, int dest);
void extractStartDestinationStreets(char buffer[], char start[], char destination[]);
int getIdStreet(MYSQL *traficDataBase, char street[]);
void getStreetById(MYSQL *traficDataBase, int id, char street[]);

allSocketDesc clientsSocketDescriptors;
int main ()
{
	clearAccidentTrafficJamTable();
	
	struct sockaddr_in server;
	struct sockaddr_in from;	
	int nr;
	int sd;
	pthread_t th[100];
	pthread_mutex_init(&lock, NULL);
	int i=0;

	if ((sd = socket (PF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror ("[server]Error at socket().\n");
		return errno;
	}

	int on=1;
	setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)); //set socket option 

	bzero (&server, sizeof (server));
	bzero (&from, sizeof (from));

	server.sin_family = AF_INET;	
	server.sin_addr.s_addr = htonl (INADDR_ANY);
	server.sin_port = htons (PORT);

	if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
	{
		perror ("[server]Error at bind().\n");
		return errno;
	}

	if (listen (sd, 20) == -1)
	{
		perror ("[server]Error at listen().\n");
		return errno;
	}

	while (1)
	{
		int client;
     	thData * td; //parameter for thread function   
      	int length = sizeof (from);

      	printf ("[server]Waiting at port %d...\n",PORT);
      	fflush (stdout);

	    if ( (client = accept (sd, (struct sockaddr *) &from, &length)) < 0)
	    {
	     	perror ("[server]Error at accept().\n");
	      	continue;
	    }

	    td=(struct thData*)malloc(sizeof(struct thData));	
	    td->idThread=i++;
	    td->cl=client;
	    pthread_create(&th[i], NULL, &handleClient, td);	     
 	}
	pthread_mutex_destroy(&lock);
}			
void *handleClient(void * arg)
{		
	struct thData serveClient; 
	serveClient = *((struct thData*)arg);	
	clientsSocketDescriptors.sd[serveClient.idThread] = serveClient.cl;
    clientsSocketDescriptors.activ[serveClient.idThread] = 1;
	printf ("[thread %d] Waiting for a message...\n", serveClient.idThread);
	fflush (stdout);		 
	pthread_detach(pthread_self());

	char verifyCommand[MAX_COMMAND_LENG];
	char message[MAX_REPLY_LENG];
	char clientPacket[MAX_REPLY_LENG];
	char command[MAX_COMMAND_LENG];
	char commandPacket[MAX_COMMAND_LENG];
	char syntaxError[100] = "Syntax error. Respect the format"
	"\nType \"help\" for details.";
	char passAccidentMessage[200] = "There is a car accident reported on this street."
									"\nThe speed limit from the time you pass an accident sign until you have passed the crash site is 20km\\h.";
	char passTrafficJamMessage[200] = "There is a traffic jam reported on this street."
									"The recommended speed is 10km\\h";
	const char userNotLoggedError[50] = "Please login and try again.";
	int messageLength;
	int commandLength;
	int check;
	int quit = 0;
	int loggedIn = 0;
	char username[20];
	char copyUsername[20];
	char thisCarStreet[100];
	int lastCommandIsSpeed = 0;
	int accident;
	int validStreet = 0;

	MYSQL *dataBase = mysql_init(NULL);

	if (dataBase == NULL)
	{
		printf("%s\n", mysql_error(dataBase));
		exit(1);
	}
	if (mysql_real_connect(dataBase, "localhost", "root", "root", "TrafficManagement", 0, NULL, 0) == NULL)
	{
		errorDB(dataBase);
	}

	while(1)
	{
		bzero (command, MAX_COMMAND_LENG);
		bzero (message, MAX_REPLY_LENG);
		bzero (commandPacket, MAX_COMMAND_LENG);
		lastCommandIsSpeed = 0;
		accident = 0;
		validStreet = 0;
		if ((check = recv (serveClient.cl, &commandPacket, MAX_COMMAND_LENG, 0)) == 0)
		{
			clientsSocketDescriptors.activ[serveClient.idThread] = 0;
			printf("[Thread %d] socket closed\n", serveClient.idThread);
			close (serveClient.cl);
			return(NULL);	
		}
		else if (check < 0)
		{
			printf("[Thread %d]\n",serveClient.idThread);
			perror ("Error at recv(). Cause ");
			close (serveClient.cl);
			return(NULL);
		}
		else
		{
			printf ("[Thread %d]Message has been received...%s\n",serveClient.idThread, commandPacket);
		}
		decodePacket(commandPacket, command, &commandLength);

		if(strlen(command) == commandLength)
		{
			if(verifyNrArguments(command) == 2)
			{
				extractArgument(command, 1, verifyCommand);
			}
			else
			{
				strcpy(verifyCommand, command);
				removeSpacesAndNewLine(verifyCommand);
			}
			notCaseSensitive(verifyCommand);
			if(strcmp(verifyCommand, "quit") == 0)
			{
				loggedIn = 0;
				quit = 1;
				strcpy(message, "Drive safe!");
			}
			else if(strcmp(verifyCommand, "help") == 0)
			{
				if(strlen(verifyCommand) == countLetters(command))
				{
					char help[2000] = "Traffic Management System available commands"
					"\n--Warning! You must be logged in to use these commands."
					"\n--Exceptions: quit, login, register, help"
					"\n--Warning! This program is whitespace sensitive."
					"\nlogin : <username> (you must have an account)"
					"\nregister : <username> (to make an account)"
					"\nlogout (to disconnect current account)"
					"\ndelete-account (to delete your account)"
					"\nquit (quit Traffic Management System)"
					"\nspeed : <enter your current speed without the unit measurement>"
					"\nstreet : <enter the name of the street>"
					"\naccident : <street where accident took place> (to report an accident)" //what to do (c) https://www.geico.com/living/driving/auto/car-safety-insurance/what-to-do-if-you-witness-a-car-accident/comment-page-7/
					"\nno-accident : <street where there is no accident> (to delete an accident)"
					"\nall-accidents (to see reported accidents)" 
					"\ntraffic-jam : <street> (to report a traffic jam)" //what to do (c) https://www.statefarm.com/simple-insights/auto-and-vehicles/driving-tips-for-traffic-jams
					"\nno-traffic-jam : <street> (location where there is no traffic jam)"
					"\nall-traffic-jams (to see reported traffic jams)"
					"\nnewsletter : {<weather>, <sport>, <fuel>} (to get information about weather, sport news or fuel prices)"
					"\nget-directions : <start street>-><destination street> (to see route between start street and destination street, please put \"->\" between them."
					"\nhelp (lists the complete set of available commands)"
					"\n<> you must enter a value without \"<>\""
					"\n{} you can type one or more options ";
					strcpy(message, help);
				}
				else
				{
					strcpy(message, syntaxError);
				}
			}
			else if(strcmp(verifyCommand, "login") == 0)
			{
				if(!loggedIn)
				{
					if(verifyNrArguments(command) != 2)
					{
						strcpy(message, syntaxError);
						strcat(message, "Please enter your username.");
					}
					else
					{
						char sayHello[50] = "\nHello, ";
						
						extractArgument(command, 2, username);
						removeSpacesAndNewLine(username);
						if(strlen(username) >= 50)
						{
							strcpy(message, "Error: username too long.");
						}
						else
						{
							int checkUsername = verifyUsernameDB(dataBase, username);
							if(checkUsername)
							{
								loggedIn = 1;
								strcpy(message, "Connected");
								strcat(sayHello, username);
								strcat(message, sayHello);
								strcpy(copyUsername, username);
								char query[200];
								sprintf(query, "UPDATE users SET currentSpeed = NULL WHERE currentSpeed IS NOT NULL and username like '%s'", username);
								if (mysql_query(dataBase, query))
								{
									errorDB(dataBase);
								}
								bzero(query, sizeof(query));
								sprintf(query, "UPDATE users SET street = NULL WHERE street IS NOT NULL and username like '%s'", username);

								if (mysql_query(dataBase, query))
								{
									errorDB(dataBase);
								}
							}
							else
							{
								strcpy(message, "Invalid username. Try another username or use register command.");
							}
						}
					}
				}
				else
				{
					strcpy(message, "User currently connected. Try logout command first.");
				}		
			}
			else if(strcmp(verifyCommand, "delete-account") == 0)
			{
				if(strlen(verifyCommand) == countLetters(command))
				{
					if(loggedIn)
					{
						char query[200];
						sprintf(query, "DELETE FROM users WHERE username like '%s'", username);
						if (mysql_query(dataBase, query)) 
						{
							errorDB(dataBase);
						}
						bzero(username, sizeof(username));
						bzero(copyUsername, sizeof(copyUsername));
						strcpy(message, "Your account has been deleted successfully.");
						loggedIn = 0;
					}
					else
					{
						strcpy(message, userNotLoggedError);
					}
				}
				else
				{
					strcpy(message, syntaxError);
				}
			}
			else if(strcmp(verifyCommand, "logout") == 0)
			{
				if(strlen(verifyCommand) == countLetters(command))
				{
					if(loggedIn)
					{
						loggedIn = 0;
						bzero(username, sizeof(username));
						bzero(copyUsername, sizeof(copyUsername));
						strcpy(message, "Disconnected");
					}
					else
					{
						loggedIn = 0;
						strcpy(message, "There is no connected account.");
					}
				}
				else
				{
					strcpy(message, syntaxError);
				}
			}
			else if(strcmp(verifyCommand, "register") == 0)
			{
				if(!loggedIn)
				{
					if(verifyNrArguments(command) != 2)
					{
						strcpy(message, syntaxError);
						strcat(message, "Please enter your username.");
					}
					else
					{
						extractArgument(command, 2, username);
						removeSpacesAndNewLine(username);
						int existsUsername = verifyUsernameDB(dataBase, username);
						if(!existsUsername)
						{
							char query[500];
							int userid = generateUserId();
							loggedIn = 1;
							userid++;
							sprintf(query,"INSERT INTO users(id, username) VALUES(%d, '%s')",userid, username);
							if (mysql_query(dataBase, query)) 
							{
								errorDB(dataBase);
							}
							strcpy(message, "You have successfully created your account");
							strcpy(copyUsername, username);
						}
						else
						{
							strcpy(message, "Username taken. Try another.");
						}
					}
				}
				else
				{
					strcpy(message, "User currently connected. Try logout command first.");
				}	
			}
			else if(strcmp(verifyCommand, "speed") == 0)
			{
				if(loggedIn)
				{
					if(verifyNrArguments(command) != 2)
					{
						strcpy(message, syntaxError);
						strcat(message, "Please enter your current speed.");
					}
					else
					{
						char charThisCarSpeed[5];
						char query[500];
						char charSpeed[10];
						int trafficJamFlag = 0;
						int accidentFlag = 0;
						int maxSpeed;
						extractArgument(command, 2, charThisCarSpeed);
						removeSpacesAndNewLine(charThisCarSpeed);
						strcpy(username, copyUsername);
						if(isNumber(charThisCarSpeed))
						{
							int thisCarSpeed = atoi(charThisCarSpeed);
							sprintf(query,"UPDATE users SET currentSpeed = %d WHERE username like '%s'", thisCarSpeed, username);
							if (mysql_query(dataBase, query)) 
							{
								errorDB(dataBase);
							}

							bzero(query, sizeof(query));
							strcpy(username, copyUsername);
							sprintf(query,"SELECT EXISTS(SELECT * FROM users WHERE street is not null and username like '%s')", username);
							if(checkIfStreetExists(dataBase, query))
							{
								bzero(query, sizeof(query));
								strcpy(username, copyUsername);
								sprintf(query,"SELECT street FROM users WHERE username like '%s'", username);
								if (mysql_query(dataBase, query))
								{
									errorDB(dataBase);
								}

								MYSQL_RES *result = mysql_store_result(dataBase);

								if (result == NULL)
								{
									errorDB(dataBase);
								}

								MYSQL_ROW row = mysql_fetch_row(result);
								strcpy(thisCarStreet, row[0]);
								mysql_free_result(result);

								char query1[200];
								char query2[200];
								sprintf(query1,"SELECT EXISTS(SELECT * FROM trafficJam WHERE TRIM(street) LIKE '%s')", thisCarStreet);
								sprintf(query2,"SELECT EXISTS(SELECT * FROM accident WHERE TRIM(street) LIKE '%s')", thisCarStreet);
								if(checkIfStreetExists(dataBase, query1)) //verify trafficJam
								{
									trafficJamFlag = 1;
									maxSpeed = 10;
								}
								else if(checkIfStreetExists(dataBase, query2)) //verify accident
								{
									accidentFlag = 1;
									maxSpeed = 20;
								}
								else
								{
									bzero(query, sizeof(query));
									sprintf(query, "SELECT speedLimit FROM speed WHERE TRIM(street) LIKE '%s'", thisCarStreet);
									if (mysql_query(dataBase, query)) 
									{
										errorDB(dataBase);
									}
									result = mysql_store_result(dataBase);

									if (result == NULL)
									{
										errorDB(dataBase);
									}

									row = mysql_fetch_row(result);
									strcpy(charSpeed, row[0]);
									maxSpeed = atoi(charSpeed);
									mysql_free_result(result);
								}
								if(thisCarSpeed >= maxSpeed)
								{
									strcpy(message, "Warning:overspeed detected.\n");
								}
								else
								{
									strcpy(message, "Regulatory speed.\n");
								}
								if(trafficJamFlag)
								{
									strcat(message, passTrafficJamMessage);
								}
								else if(accidentFlag)
								{
									strcat(message, passAccidentMessage);
								}
								else
								{
									strcat(message, "Speed limit on this street is: ");
									strcat(message, charSpeed);
									strcat(message, " km\\h");
								}
								lastCommandIsSpeed = 1;
							}
							else
							{
								strcpy(message, "Please enter your currently street first. Type \"help\" for details.");
							}
						}
						else
						{
							strcpy(message, "Invalid speed. Type \"help\" for details.");
						}	
					}
				}
				else
				{
					strcpy(message, userNotLoggedError);
				}
			}
			else if(strcmp(verifyCommand, "street") == 0)
			{
				if(loggedIn)
				{
					if(verifyNrArguments(command) != 2)
					{
						strcpy(message, syntaxError);
						strcat(message, "Please enter the name of the street.");
					}
					else
					{
						char query[500];
						extractArgument(command, 2, thisCarStreet);

						if(!verifyComposeNameStreet(thisCarStreet))
						{
							removeSpacesAndNewLine(thisCarStreet);
						}
						else
						{
							removeSpacesAndNewLineComposedStreet(thisCarStreet);
						}
						
						bzero(query, sizeof(query));
						sprintf(query,"SELECT EXISTS(SELECT * FROM speed WHERE street like '%s')", thisCarStreet);
						if(checkIfStreetExists(dataBase, query))
						{
							bzero(query, sizeof(query));
							strcpy(username, copyUsername);
							sprintf(query,"UPDATE users SET street = '%s' WHERE username like '%s'", thisCarStreet, username);
							if (mysql_query(dataBase, query)) 
							{
								errorDB(dataBase);
							}
							strcpy(message, "Location updated successfully.");

							char query1[200];
							char query2[200];
							sprintf(query1,"SELECT EXISTS(SELECT * FROM trafficJam WHERE TRIM(street) LIKE '%s')", thisCarStreet);
							sprintf(query2,"SELECT EXISTS(SELECT * FROM accident WHERE TRIM(street) LIKE '%s')", thisCarStreet);
							if(checkIfStreetExists(dataBase, query1)) //verify trafficJam
							{
								strcat(message, "\n");
								strcat(message, passTrafficJamMessage);
							}
							else if(checkIfStreetExists(dataBase, query2)) //verify accident
							{
								strcat(message, "\n");
								strcat(message, passAccidentMessage);
							}
							validStreet = 1;
						}
						else
						{
							strcpy(message, "Invalid street, please try again");
						}
					}
				}
				else
				{
					strcpy(message, userNotLoggedError);
				}
			}
			else if(strcmp(verifyCommand, "accident") == 0)
			{
				if(loggedIn)
				{
					if(verifyNrArguments(command) != 2)
					{
						strcpy(message, syntaxError);
						strcat(message, "Please enter where the accident took place.");
					}
					else
					{
						char accidentStreet[100];
						char query[200];
						extractArgument(command, 2, accidentStreet);

						if(!verifyComposeNameStreet(accidentStreet))
						{
							removeSpacesAndNewLine(accidentStreet);
						}
						else
						{
							removeSpacesAndNewLineComposedStreet(accidentStreet);
						}
						sprintf(query,"SELECT EXISTS(SELECT * FROM speed WHERE street like '%s')", accidentStreet);
						if(checkIfStreetExists(dataBase, query))
						{
							accident = 1;
							char carAccident[500] = "\nWhat to do:"
							"\nEnsure Your Safety First"
							"\nCall 112"
							"\nCheck On The Victims"
							"\nStabilize The Vehicles (If You Can)"
							"\nGive a statement and provide your contact information";
							char warning[100] = "\nWarning!!!There is a car accident on ";
							strcat(warning, accidentStreet);
							strcat(warning,  " street");
							strcpy(message, warning);
							strcat(message, carAccident);
							messageLength = strlen(message);
							makePacket(message, quit, loggedIn, lastCommandIsSpeed, accident, validStreet, messageLength, clientPacket);

							bzero(query, sizeof(query));
							sprintf(query,"SELECT EXISTS(SELECT * FROM accident WHERE street like '%s')", accidentStreet);
							if(!checkIfStreetExists(dataBase, query))
							{
								sprintf(query,"INSERT INTO accident(street) VALUES('%s')", accidentStreet);
								if (mysql_query(dataBase, query)) 
								{
									errorDB(dataBase);
								}
								//message for all connected users
								for(int i = 0; i < 20; i++)
								{
									if(clientsSocketDescriptors.activ[i] == 1 && i != serveClient.idThread)
									{
										if((check = send(clientsSocketDescriptors.sd[i], &clientPacket, MAX_REPLY_LENG, 0)) < 0)
										{
											printf("[Thread %d] ",serveClient.idThread);
											perror ("[Thread]Error at send().\n");
										}
										else if(check == 0)
										{
											clientsSocketDescriptors.activ[serveClient.idThread] = 0;
											printf("[Thread %d] socket closed\n", serveClient.idThread);
											close (serveClient.cl);
											return(NULL);
										}
										else
										{
											printf("[Thread %d]Sending the message...%s\n",serveClient.idThread, clientPacket);
											printf ("[Thread %d]Message has been successfully delivered to everyone...\n", serveClient.idThread);
											sleep(1);	
											if(quit)
											{
												clientsSocketDescriptors.activ[serveClient.idThread] = 0;
												printf("[Thread %d] socket closed\n", serveClient.idThread);
												close (serveClient.cl);
												return(NULL);	
											}
										}
									}
								}
							}
							
							//message for driver that reported the accident
							strcpy(message, "Thank you for reporting the accident.");
							strcat(message, carAccident);
							messageLength = strlen(message);
							makePacket(message, quit, loggedIn, lastCommandIsSpeed, 0, validStreet, messageLength, clientPacket);
							if((check = send(serveClient.cl, &clientPacket, MAX_REPLY_LENG, 0)) < 0)
							{
								printf("[Thread %d] ",serveClient.idThread);
								perror ("[Thread]Error at send().\n");
							}
							else if(check == 0)
							{
								clientsSocketDescriptors.activ[serveClient.idThread] = 0;
								printf("[Thread %d] socket closed\n", serveClient.idThread);
								close (serveClient.cl);
								return(NULL);
							}
							else
							{
								printf("[Thread %d]Sending the message...%s\n",serveClient.idThread, clientPacket);
								printf ("[Thread %d]Message has been successfully delivered...\n", serveClient.idThread);
								sleep(1);	
								if(quit)
								{
									clientsSocketDescriptors.activ[serveClient.idThread] = 0;
									printf("[Thread %d] socket closed\n", serveClient.idThread);
									close (serveClient.cl);
									return(NULL);	
								}
							}
						}
						else
						{
							strcpy(message, "Invalid street, please try again.");
						}
					}
				}
				else
				{
					strcpy(message, userNotLoggedError);
				}
			}
			else if(strcmp(verifyCommand, "no-accident") == 0)
			{
				if(loggedIn)
				{
					if(verifyNrArguments(command) != 2)
					{
						strcpy(message, syntaxError);
						strcat(message, "Please enter the street where there is no car accident.");
					}
					else
					{
						char accidentStreet[100];
						char query[200];
						extractArgument(command, 2, accidentStreet);

						if(!verifyComposeNameStreet(accidentStreet))
						{
							removeSpacesAndNewLine(accidentStreet);
						}
						else
						{
							removeSpacesAndNewLineComposedStreet(accidentStreet);
						}
						sprintf(query,"SELECT EXISTS(SELECT * FROM accident WHERE street like '%s')", accidentStreet);
						if(checkIfStreetExists(dataBase, query))
						{
							bzero(query, sizeof(query));
							sprintf(query, "DELETE FROM accident WHERE street like '%s'", accidentStreet);
							if (mysql_query(dataBase, query)) 
							{
								errorDB(dataBase);
							}
							strcpy(message, "Accident has been deleted successfully.");
						}
						else
						{
							strcpy(message, "There is no car accident reported on this street");
						}
					}
				}
				else
				{
					strcpy(message, userNotLoggedError);
				}
			}
			else if(strcmp(verifyCommand, "all-accidents") == 0)
			{
				if(strlen(verifyCommand) == countLetters(command))
				{
					if(loggedIn)
					{
						strcpy(message, "Reported accidents:\n"); 
						char table[20] = "accident";
						getNewsletterInfo(message, dataBase, table);
						strcat(message, "Please pay attention when you are driving.");
					}
					else
					{
						strcpy(message, userNotLoggedError);
					}
				}
				else
				{
					strcpy(message, syntaxError);
				}
			}
			else if(strcmp(verifyCommand, "traffic-jam") == 0)
			{
				if(loggedIn)
				{
					if(verifyNrArguments(command) != 2)
					{
						strcpy(message, syntaxError);
						strcat(message, "Please enter where the accident took place.");
					}
					else
					{
						char trafficJamStreet[100];
						char query[200];
						extractArgument(command, 2, trafficJamStreet);

						if(!verifyComposeNameStreet(trafficJamStreet))
						{
							removeSpacesAndNewLine(trafficJamStreet);
						}
						else
						{
							removeSpacesAndNewLineComposedStreet(trafficJamStreet);
						}
						sprintf(query,"SELECT EXISTS(SELECT * FROM speed WHERE street like '%s')", trafficJamStreet);
						if(checkIfStreetExists(dataBase, query))
						{
							char trafficJamTips[2000] = "\n---Make smart driving decisions in heavy traffic---\n"
													"___________________________________________________\n"
													"\n> Avoid aggressive driving and weaving from lane to lane."
													"\n> Keep a safe distance, at least three seconds, between you and the vehicle ahead of you. This will help you avoid frequent braking and rear end collisions."
													"\n> Watch the traffic ahead closely. When cars in front brake, take your foot off the accelerator to slow down gradually before you brake. Be cautious not to brake abruptly."
													"\n> Stay focused. Keep your mind on driving by avoiding distractions such as eating or using your cell phone.\n"
													"\n---Follow driving fundamentals in a traffic jam---\n"
													"____________________________________________________\n"
													"\n> Use your blinkers when changing lanes or merging. Most state laws require activating blinkers at least 100 feet before merging or turning, but some states require at least 200 feet."
													"\n> Use your mirrors to monitor the areas around your car. Perform head checks to check your blind spots before changing lanes or merging."
													"\n> Use the length of the acceleration lane to merge safely into traffic."
													"\n> Be responsible with technology, and plan ahead. Before you start your trip, check traffic apps on your phone or listen to radio reports to avoid congested areas. If you're worried about encountering heavy traffic during your commute, take time before you leave to map out an alternate route on your GPS."
													"\n> On narrow lanes, make sure you stay centered in your lane and don’t cut corners.\n"
													"\nThank you for reporting a traffic jam.";
							strcpy(message, trafficJamTips);
							bzero(query, sizeof(query));
							sprintf(query,"SELECT EXISTS(SELECT * FROM trafficJam WHERE street like '%s')", trafficJamStreet);
							if(!checkIfStreetExists(dataBase, query))
							{
								bzero(query, sizeof(query));
								sprintf(query,"INSERT INTO trafficJam(street) VALUES('%s')", trafficJamStreet);
								if (mysql_query(dataBase, query)) 
								{
									errorDB(dataBase);
								}
							}
						}
						else
						{
							strcpy(message, "Invalid street, please try again.");
						}
					}
				}
				else
				{
					strcpy(message, userNotLoggedError);
				}
			}
			else if(strcmp(verifyCommand, "no-traffic-jam") == 0)
			{
				if(loggedIn)
				{
					if(verifyNrArguments(command) != 2)
					{
						strcpy(message, syntaxError);
						strcat(message, "Please enter the street where there is no car accident.");
					}
					else
					{
						char trafficJamStreet[100];
						char query[200];
						extractArgument(command, 2, trafficJamStreet);

						if(!verifyComposeNameStreet(trafficJamStreet))
						{
							removeSpacesAndNewLine(trafficJamStreet);
						}
						else
						{
							removeSpacesAndNewLineComposedStreet(trafficJamStreet);
						}
						sprintf(query,"SELECT EXISTS(SELECT * FROM trafficJam WHERE street like '%s')", trafficJamStreet);
						if(checkIfStreetExists(dataBase, query))
						{
							bzero(query, sizeof(query));
							sprintf(query, "DELETE FROM trafficJam WHERE street like '%s'", trafficJamStreet);
							if (mysql_query(dataBase, query)) 
							{
								errorDB(dataBase);
							}
							strcpy(message, "Traffic jam has been deleted successfully.");
						}
						else
						{
							strcpy(message, "There is no traffic jam reported on this street");
						}
					}
				}
				else
				{
					strcpy(message, userNotLoggedError);
				}
			}
			else if(strcmp(verifyCommand, "all-traffic-jams") == 0)
			{
				if(strlen(verifyCommand) == countLetters(command))
				{
					if(loggedIn)
					{
						strcpy(message, "Reported traffic jams:\n"); 
						char table[20] = "trafficJam";
						getNewsletterInfo(message, dataBase, table);
						strcat(message, "Please pay attention when you are driving.");
					}
					else
					{
						strcpy(message, userNotLoggedError);
					}
				}
				else
				{
					strcpy(message, syntaxError);
				}
			}
			else if(strcmp(verifyCommand, "newsletter") == 0)
			{
				if(loggedIn)
				{
					if(verifyNrArguments(command) < 2)
					{
						strcpy(message, syntaxError);
						strcat(message, "\nPlease enter at least one option.");
					}
					else
					{
						char option1[20];
						char option2[20];
						char option3[20];
						int flagOption1 = 0;
						int flagOption2 = 0;
						int flagOption3 = 0;
						//0 = not chosen option, 1 = correct chosen option, 2 = wrong chosen option
						if(parseNewsletter(command, option1, option2, option3))
						{
							removeSpacesAndNewLine(option1);
							removeSpacesAndNewLine(option2);
							removeSpacesAndNewLine(option3);
							strcpy(message, "You have subscribed to: \n");

							if(strlen(option1) != 0)
							{
								if((strcmp(option1, "weather") == 0) || (strcmp(option1, "fuel") == 0) || (strcmp(option1, "sport") == 0))
								{
									flagOption1 = 1;
									strcat(message, option1);
									strcat(message, "\n");
									getNewsletterInfo(message, dataBase, option1);
								}
								else
								{
									flagOption1 = 2;
								}
						
							}
							if(strlen(option2) != 0)
							{
								if((strcmp(option2, "weather") == 0) || (strcmp(option2, "fuel") == 0) || (strcmp(option2, "sport") == 0))
								{
									flagOption2 = 1;
									strcat(message, "\n");
									strcat(message, option2);
									strcat(message, "\n");
									getNewsletterInfo(message, dataBase, option2);
								}
								else
								{
									flagOption2 = 2;
								}
							}
							if(strlen(option3) != 0)
							{
								if((strcmp(option3, "weather") == 0) || (strcmp(option3, "fuel") == 0) || (strcmp(option3, "sport") == 0))
								{
									flagOption3 = 1;
									strcat(message, "\n");
									strcat(message, option3);
									strcat(message, "\n");
									getNewsletterInfo(message, dataBase, option3);
								}
								else
								{
									flagOption3 = 2;
								}
							}
							if((flagOption1 == 2 && flagOption2 == 2 && flagOption3 == 2) || (flagOption1 == 2 && flagOption2 == 2 && flagOption3 == 0) || (flagOption1 == 2 && flagOption2 == 0 && flagOption3 == 0))
							{
								strcpy(message, "Invalid option(all of them): check your spelling or try \"help\" for details.");
							}
							else if(flagOption1 == 2 || flagOption2 == 2 || flagOption3 == 2)
							{
								char errorOption[100] = "Invalid option(";
								if(flagOption1 == 2)
								{
									strcat(errorOption, option1);
								}
								if(flagOption2 == 2)
								{
									if(flagOption1 == 2)
									{
										strcat(errorOption, ", ");
									}
									strcat(errorOption, option2);
								}
								if(flagOption3 == 2)
								{
									if(flagOption1 == 2 || flagOption2 == 2)
									{
										strcat(errorOption, ", ");
									}
									strcat(errorOption, option3);
								}
								strcat(errorOption, "): check your spelling or try \"help\" for details.");
								strcat(message, "\n");
								strcat(message, errorOption);
							}
							if(strlen(option1) == 0 && strlen(option2) == 0 && strlen(option3) == 0)
							{
								strcpy(message, syntaxError);
								strcat(message, "\nPlease enter at least one option.");
							}
						}
						else
						{
							strcpy(message, "Invalid number of arguments. Type \"help\" for details.");
						}
					}
				}
				else
				{
					strcpy(message, userNotLoggedError);
				}
			}
			else if(strcmp(verifyCommand, "get-directions") == 0)
			{
				if(loggedIn)
				{
					if(verifyNrArguments(command) != 2)
					{
						strcpy(message, syntaxError);
						strcat(message, "Please enter the name of the streets.");
					}
					else
					{
						char streets[100];
						char startStreet[20];
						char destinationStreet[20];
						bzero(streets, sizeof(streets));
						bzero(startStreet, sizeof(startStreet));
						bzero(destinationStreet, sizeof(destinationStreet));
						char route[30];
						int idStartStreet;
						int idDestinationStreet;
						extractArgument(command, 2, streets);
						extractStartDestinationStreets(streets, startStreet, destinationStreet);
						if(verifyComposeNameStreet(destinationStreet))
						{
							removeSpacesAndNewLineComposedStreet(destinationStreet);
						}
						else
						{
							removeSpacesAndNewLine(destinationStreet);
						}
						if(strlen(startStreet) == 0 || strlen(destinationStreet) == 0)
						{
							strcpy(message, "Invalid streets. Respect the format.\nType \"help\" for details.");
						}
						else
						{
							char queryStartStreet[200];
							char queryDestStreet[200];
							sprintf(queryStartStreet,"SELECT EXISTS(SELECT * FROM speed WHERE street like '%s')", startStreet);
							sprintf(queryDestStreet,"SELECT EXISTS(SELECT * FROM speed WHERE street like '%s')", destinationStreet);
							
							if(checkIfStreetExists(dataBase, queryStartStreet) && checkIfStreetExists(dataBase, queryDestStreet))
							{
								int adjacencyMatrix[37][37] = { {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,7,0,0,0,0,0,0,0,0,0,0,8,0,10,3,0,0,0,0},
									{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,4},
									{0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,3,0,0,0,4,0},
									{0,0,0,0,7,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,0,0,0,0,14,0,0,0,0,0,0,0},
									{0,0,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,0},
									{0,0,2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,0,0,0,0,0,0,0,0,0,0,0,0,0},
									{0,0,0,0,0,0,0,0,0,0,0,0,0,20,13,0,0,0,0,0,0,0,0,0,0,0,0,0,25,0,0,0,0,0,0,30,0},
									{0,0,0,0,0,0,0,0,19,5,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,0,0,0,0,0,0,0,0},
									{0,0,0,0,0,0,0,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,26,0,0,0,0,12,0,0,0,0,0,0,0},
									{0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0},
									{0,0,0,0,0,0,0,15,0,0,0,5,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
									{0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0},
									{0,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,15,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0},
									{0,0,0,0,0,0,20,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
									{0,0,0,0,0,0,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,35,0,0,0,0,0,0,20,0},
									{0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,18,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0},
									{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,0,15,0,0,0,19,0,0,0,0,0},
									{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,35},
									{7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
									{0,0,0,0,25,0,0,0,0,0,0,0,0,0,0,18,0,0,0,0,5,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0},
									{0,0,0,0,0,0,0,0,0,0,0,0,15,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
									{0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
									{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,50,0,0,0,0,0,0,0,0,0,0},
									{0,0,0,0,0,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
									{0,0,0,12,0,0,0,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
									{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,30,0,0,0,3,0,0},
									{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,50,0,0,0,0,0,0,0,25,0,0,0,0,0,0},
									{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,25},
									{0,0,0,0,0,0,25,16,0,0,0,10,0,0,35,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,0},
									{8,0,0,14,0,0,0,0,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,0,0,0},
									{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,30,25,0,0,0,0,0,0,0,0,0,0},
									{10,5,3,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
									{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
									{0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
									{0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0},
									{0,0,4,0,15,0,30,0,0,0,0,0,0,0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
									{0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,35,0,0,0,0,0,0,0,0,0,25,0,0,0,0,0,0,0,0,0}
								};
								idStartStreet = getIdStreet(dataBase, startStreet);
								idDestinationStreet = getIdStreet(dataBase, destinationStreet);
								idStartStreet--;
								idDestinationStreet--;
								pthread_mutex_lock(&lock);
								dijkstra(adjacencyMatrix, idStartStreet, idDestinationStreet);
								strcpy(message, "This is the shortest route: \n");
								for(int i = 0; i < indexForPath; i++)
								{
									path[i]++;
									getStreetById(dataBase, path[i], route);
									strcat(message, route);
									if(i < indexForPath - 1)
									{
										strcat(message, "---->");
									}
									bzero(route, sizeof(route));
								}
								bzero(path, sizeof(path));
								indexForPath = 0;
								pthread_mutex_unlock(&lock);
							}
							else
							{
								printf("s=%s, d=%s\n", startStreet, destinationStreet);
								strcpy(message, "Invalid streets. Respect the format.\nType \"help\" for details.");
							}
						}
					}
				}
				else
				{
					strcpy(message, userNotLoggedError);
				}
			}
			else
			{
				strcpy(message, "Incorrect command.Type \"help\" for details.");
			}
		}
		else
		{
			strcpy(message, "There was a temporary problem delivering your message. Please try again.");
		}
		if(serveClient.idThread >= 100)
		{
			printf("Server needs to be restarted.\n");
			strcpy(message, "Server error: too much clients connected.\nPlease try again later.");
		}
		if(!accident)
		{
			messageLength = strlen(message);

			makePacket(message, quit, loggedIn, lastCommandIsSpeed, accident, validStreet, messageLength, clientPacket);

			if((check = send(serveClient.cl, &clientPacket, MAX_REPLY_LENG, 0)) < 0)
			{
				printf("[Thread %d] ",serveClient.idThread);
				perror ("[Thread]Error at send().\n");
			}
			else if(check == 0)
			{
				clientsSocketDescriptors.activ[serveClient.idThread] = 0;
				printf("[Thread %d] socket closed\n", serveClient.idThread);
				close (serveClient.cl);
				return(NULL);	
			}
			else
			{
				printf("[Thread %d]Sending the message...%s\n",serveClient.idThread, clientPacket);
				printf ("[Thread %d]Message has been successfully delivered...\n", serveClient.idThread);
				sleep(1);	
				if(quit)
				{
					clientsSocketDescriptors.activ[serveClient.idThread] = 0;
					printf("[Thread %d] socket closed\n", serveClient.idThread);
					close (serveClient.cl);
					return(NULL);	
				}
			}  
		}  
	}				
}

void extractArgument(char comanda[], int index, char argument[])
{
	int lungCom = strlen(comanda);
	int i = 0, j = 0;
	if(index == 1)
	{
		while(comanda[i] != ' ' && i < lungCom)
		{
			argument[j++] = comanda[i++];
		}

	}
	if(index == 2)
	{
		while(comanda[i] != ':' && i < lungCom)
		{
			i++;
		}
		i++;
		while(i < lungCom)
		{
			argument[j++] = comanda[++i];
		}
	}
	argument[j] = '\0';
}
int verifyNrArguments(char comanda[])
{
	int lungCom = strlen(comanda);
	int flag = 0;
	int argumentsNum = 0;
	int i = 0;
	while(i < lungCom && comanda[i] != ':')
	{
		i++;
	}
	argumentsNum = 1;
	while(i < lungCom)
	{
		flag = 1;
		if(comanda[i] == ',')
		{
			argumentsNum++;
		}
		i++;
	}
	if(flag)
	{
		argumentsNum++;
	}
	return argumentsNum;
}

void removeSpacesAndNewLine(char comanda[])
{
	int lungCom = strlen(comanda);
	int i = 0;
	while(i <= lungCom)
	{
		if(comanda[i] == ' ' || comanda[i] == '\n')
		{
			comanda[i] = '\0';
		}
		i++;
	}
}
int parseNewsletter(char userCommand[], char op1[], char op2[], char op3[])
{
	int i = 0, j = 0;
	int commLeng = strlen(userCommand);
	int flag = 1;
	while(i < commLeng && userCommand[i] != ':')
	{
		i++;
	}
	i++;
	while(i < commLeng)
	{
		if(userCommand[i] == ',')
		{
			if(flag == 1)
			{
				op1[j] = '\0';
			}
			else if(flag == 2)
			{
				op2[j] = '\0';
			}
			j = 0;
			i++;
			flag++;
		}
		if(userCommand[i] == ' ')
		{
			i++;
		}
		if(flag == 1)
		{
			op1[j++] = userCommand[i++];
		}
		else if(flag == 2)
		{
			op2[j++] = userCommand[i++];
		}
		else if(flag == 3)
		{
			op3[j++] = userCommand[i++];
		}
		else
		{
			return 0;
		}
	}
	if(flag == 1)
	{
		op2[0] = '\0';
		op3[0] = '\0';
	}
	else if(flag == 2)
	{
		op3[0] = '\0';
	}
	else
	{
		op3[j] = '\0';
	}
	return 1;
}
int verifyComposeNameStreet(char streetName[])
{
	int commLeng = strlen(streetName);
	int spaces = 0;
	int i = 0;
	while(i < commLeng)
	{
		if(streetName[i] == ' ' && streetName[i] != streetName[commLeng-1])
		{
			spaces++;
		}
		i++;
	}
	if(spaces > 0)
	{
		return 1;
	}
	return 0;
}
void removeSpacesAndNewLineComposedStreet(char streetName[])
{
	int commLeng = strlen(streetName);
	if(streetName[commLeng - 1] == ' ' || streetName[commLeng - 1] == '\n')
	{
		streetName[commLeng - 1] = '\0';
	}
}
void getNewsletterInfo(char buffer[], MYSQL *traficDataBase, char option[])
{
	char query[100] = "SELECT * FROM ";
	int leng = strlen(buffer);
	strcat(query, option);
	if (mysql_query(traficDataBase, query))
	{
		errorDB(traficDataBase);
	}

	MYSQL_RES *result = mysql_store_result(traficDataBase);

	if (result == NULL)
	{
		errorDB(traficDataBase);
	}

	int numFields = mysql_num_fields(result);

	MYSQL_ROW row;

	while ((row = mysql_fetch_row(result)))
	{
		for(int i = 0; i < numFields; i++)
		{
			strcat(buffer, row[i] ? row[i] : "NULL");
			strcat(buffer, " ");
		}
		strcat(buffer, "\n");
	}
	if(leng == strlen(buffer))
	{
		strcat(buffer, "No results\n");
	}
	mysql_free_result(result);
}
int checkIfStreetExists(MYSQL *traficDataBase, char mysqlQuery[])
{	
	if (mysql_query(traficDataBase, mysqlQuery))
	{
		errorDB(traficDataBase);
	}

	MYSQL_RES *result = mysql_store_result(traficDataBase);

	if (result == NULL)
	{
		errorDB(traficDataBase);
	}

	MYSQL_ROW row = mysql_fetch_row(result);
	char checkIfStreetExists[5];
	strcpy(checkIfStreetExists, row[0]);
	int checkTheStreetDB = atoi(checkIfStreetExists);
	mysql_free_result(result);
	return checkTheStreetDB;
}
void makePacket(char commandMessage[], int quit, int loggedIn, int lastCommandIsSpeed, int ifAccident, int ifValidStreet, int length, char packet[])
{
	char intToChar[10];
	sprintf(intToChar, "%d", quit);
	strcpy(packet, intToChar);
	strcat(packet, "#");
	bzero(intToChar, sizeof(intToChar));
	sprintf(intToChar, "%d", loggedIn);
	strcat(packet, intToChar);
	strcat(packet, "#");
	bzero(intToChar, sizeof(intToChar));
	sprintf(intToChar, "%d", lastCommandIsSpeed);
	strcat(packet, intToChar);
	strcat(packet, "#");
	bzero(intToChar, sizeof(intToChar));
	sprintf(intToChar, "%d", ifAccident);
	strcat(packet, intToChar);
	strcat(packet, "#");
	bzero(intToChar, sizeof(intToChar));
	sprintf(intToChar, "%d", ifValidStreet);
	strcat(packet, intToChar);
	strcat(packet, "#");
	bzero(intToChar, sizeof(intToChar));
	sprintf(intToChar, "%d", length);
	strcat(packet, intToChar);
	strcat(packet, "#");
	strcat(packet, commandMessage);
	strcat(packet, "#");
}
void errorDB(MYSQL *thisDataBase)
{
	printf("%s\n", mysql_error(thisDataBase));
	mysql_close(thisDataBase);
	exit(1);
}
int verifyUsernameDB(MYSQL *thisDataBase, char name[])
{
	char query[500];
	int nrRows;
	sprintf(query,"SELECT * FROM users WHERE username like '%s'", name);
	if (mysql_query(thisDataBase, query))
	{
		errorDB(thisDataBase);
	}
	MYSQL_RES *result = mysql_store_result(thisDataBase);
	if (result == NULL)
	{
		errorDB(thisDataBase);
	}
	nrRows = mysql_num_rows(result);
	mysql_free_result(result);
	if(nrRows > 0)
	{
		return 1;
	}
	return 0;
}
int generateUserId()
{
	time_t seconds;
	seconds = time(NULL);
    return seconds%1000;
}
int isNumber(char num[])
{
	int numLeng = strlen(num);
	int i = 0;
	while(i < numLeng)
	{
		if(!isdigit(num[i]))
		{
			return 0;
		}
		i++;
	}
	return 1;
}
void decodePacket(char commandPacket[], char command[], int *commandLength)
{
  int i = 0, j = 0;
  int packetLength = strlen(commandPacket);
  char tempNum[10];
  bzero(tempNum, sizeof(tempNum));
  int countSharp = 0;
  while(i < packetLength && countSharp < 1)
  {
    if(commandPacket[i] == '#')
    {
      countSharp++;
      if(countSharp == 1)
      {
        *commandLength = atoi(tempNum);
      }
      bzero(tempNum, sizeof(tempNum));
      j = 0;
      i++;
    }
    tempNum[j++] = commandPacket[i++];
  }
  i--;
  j = 0;
  while(commandPacket[i] != '#' && i < packetLength)
  {
    command[j++] = commandPacket[i++];
  }
}
int countLetters(char command[])
{
	int leng = strlen(command);
	int i = 0;
	int spaces = 0;
	while(i < leng)
	{
		if(command[i] == ' ' || command[i] == '\n')
		{
			spaces++;
		}
		i++;
	}
	return (leng - spaces);
}
void notCaseSensitive(char word[])
{
	int leng = strlen(word);
	int i = 0;
	while(i < leng)
	{
		if(isalpha(word[i])) 
		{
			word[i] = tolower(word[i]);
		}
		i++;
	}
}
void clearAccidentTrafficJamTable()
{
	MYSQL *db = mysql_init(NULL);

	if (db == NULL)
	{
		printf("%s\n", mysql_error(db));
		exit(1);
	}
	if (mysql_real_connect(db, "localhost", "root", "root", "TrafficManagement", 0, NULL, 0) == NULL)
	{
		errorDB(db);
	}
	if (mysql_query(db, "DELETE FROM trafficJam"))
	{
		errorDB(db);
	}
	if (mysql_query(db, "DELETE FROM accident"))
	{
		errorDB(db);
	}
	mysql_close(db);
}
int minDistance(int dist[], bool sptSet[])
{
    int min = INT_MAX;
    int min_index;

    for (int v = 0; v < V; v++)
    {
        if (sptSet[v] == false && dist[v] <= min)
        {
            min = dist[v], min_index = v;
        }
    }
    return min_index;
}
void findPath(int parent[], int j)
{
    if (parent[j] == -1)
        return;

    findPath(parent, parent[j]);
    path[indexForPath++] = j;
}
void dijkstra(int graph[V][V], int src, int dest) //(c) https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/
{
    int dist[V];
    bool sptSet[V];
    int parent[V];

    for (int i = 0; i < V; i++)
    {
        parent[src] = -1;
        dist[i] = INT_MAX;
        sptSet[i] = false;
    }
    dist[src] = 0;
    for (int count = 0; count < V - 1; count++)
    {
        int u = minDistance(dist, sptSet);
        sptSet[u] = true;
        for (int v = 0; v < V; v++)
        {
            if (!sptSet[v] && graph[u][v] && dist[u] + graph[u][v] < dist[v])
            {
                parent[v] = u;
                dist[v] = dist[u] + graph[u][v];
            }

        }
    }
    path[indexForPath++] = src;
    findPath(parent, dest);
}
void extractStartDestinationStreets(char buffer[], char start[], char destination[])
{
	int leng = strlen(buffer);
	int i = 0;
	int j = 0;
	while(buffer[i] != '-' && i < leng)
	{
		start[j++] = buffer[i++];
	}
	i++;
	if(buffer[i] == '>')
	{
		j = 0;
		while(i < leng)
		{
			destination[j++] = buffer[++i];
		}
	}
}
int getIdStreet(MYSQL *traficDataBase, char street[])
{
	char query[200];
	sprintf(query, "SELECT idStreet FROM speed WHERE street like '%s'", street);
	if (mysql_query(traficDataBase, query))
	{
		errorDB(traficDataBase);
	}

	MYSQL_RES *result = mysql_store_result(traficDataBase);

	if (result == NULL)
	{
		errorDB(traficDataBase);
	}
	MYSQL_ROW row;
	row = mysql_fetch_row(result);
	int id = atoi(row[0]);
	mysql_free_result(result);
	return id;
}
void getStreetById(MYSQL *traficDataBase, int id, char street[])
{
	char query[200];
	sprintf(query, "SELECT street FROM speed WHERE idStreet=%d", id);
	if (mysql_query(traficDataBase, query))
	{
		errorDB(traficDataBase);
	}

	MYSQL_RES *result = mysql_store_result(traficDataBase);

	if (result == NULL)
	{
		errorDB(traficDataBase);
	}
	MYSQL_ROW row;
	row = mysql_fetch_row(result);
	strcpy(street, row[0]);
	mysql_free_result(result);
}