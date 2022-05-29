#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>

#define MAX_COMMAND_LENG 100
#define MAX_REPLY_LENG 6000
extern int errno;

int port;
int errorFlag;
char command[MAX_COMMAND_LENG];
char packet[MAX_COMMAND_LENG];
char serverPacket[MAX_REPLY_LENG];
char serverReply[MAX_REPLY_LENG];
int replyLength;
int quit = 0;
int redFlag = 0;
char street[30];
char speed[10];
int socketDescriptor;
int loggedIn = 0;
int timePassed = 0;
int lastCommandIsSpeed = 0;
int accident = 0;
int validStreet = 0;

void errorCheck(int error, char errorMessage[]);
void decodePacket();
void updateStreet(char street[]);
void updateSpeed(char speed[]);
void sig_handler(int signum);
void removeSpacesAndNewLine(char comanda[]);
int isNumber(char num[]);
void* recvServerReply();
void makePacket(char commandMessage[], char packet[]);

int main (int argc, char *argv[])
{
  signal(SIGALRM,sig_handler);
  struct sockaddr_in server;
  
  char displayMessage[200] = "Welcome to Traffic Management System\nType \"help\" to list all available commands.\n";
  char street[30];
  char speed[10];

  if (argc != 3)
  {
    printf ("Syntax error: please try %s <ip_server> <port>\n", argv[0]);
    return -1;
  }

  port = atoi (argv[2]);

  if ((socketDescriptor = socket (PF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror ("Socket create() failed. ");
    return errno;
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(argv[1]);
  server.sin_port = htons (port);
  
  if (connect (socketDescriptor, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
  {
    perror ("Socket connect() failed. ");
    return errno;
  }
  
  printf("%s", displayMessage);

  while(!quit)
  {
    pthread_t th1;
    pthread_t th2;
    pthread_t th3;
    if(loggedIn)
    {
      if(!redFlag)
      {
        redFlag = 1;
        alarm(60);
      }
    }
    else
    {
      redFlag = 0;
      alarm(0);
    }
    if(timePassed && loggedIn)
    {
      timePassed = 0; //reset time
      alarm(0); //deactivate alarm
      do
      {
        pthread_create(&th1, NULL, &recvServerReply, NULL);
        bzero(street, sizeof(street));
        printf(">> Please enter the street where you currently are : ");
        fflush(stdout);
        read(0, &street, sizeof(street));
        updateStreet(street);
        pthread_join(th1, NULL);
        } while(!validStreet);

      do
      {
        pthread_create(&th2, NULL, &recvServerReply, NULL);
        bzero(speed, sizeof(speed));
        printf(">> Please enter your currently speed : ");
        fflush(stdout);
        read(0, &speed, sizeof(speed));
        updateSpeed(speed);
        pthread_join(th2, NULL);
      } while(isNumber(speed) == 0);
      alarm(60);
    }
    bzero (command, MAX_COMMAND_LENG);
    pthread_create(&th3, NULL, &recvServerReply, NULL);
    printf (">> ");
    fflush (stdout);
    read(0, &command, MAX_COMMAND_LENG);

    makePacket(command, packet);
    errorCheck(errorFlag = send (socketDescriptor, &packet, MAX_COMMAND_LENG, 0), "send()");
    
    if(lastCommandIsSpeed)
    {
      timePassed = 0;
      alarm(0);
      alarm(60);
    }
    pthread_join(th3, NULL);
  }
  close (socketDescriptor);
  return 0;
}
void errorCheck(int error, char errorMessage[])
{
  if(error < 0)
  {
    printf("%s ", errorMessage);
    perror("failed. Cause ");
  }
  else if(error == 0)
  {
    perror("Socket closed. Cause");
    exit(-1);
  }
}
void decodePacket()
{
  int i = 0, j = 0;
  int packetLength = strlen(serverPacket);
  char tempNum[10];
  bzero(tempNum, sizeof(tempNum));
  int countSharp = 0;
  while(i < packetLength && countSharp < 6)
  {
    if(serverPacket[i] == '#')
    {
      countSharp++;
      if(countSharp == 1)
      {
        quit = atoi(tempNum);
      }
      else if(countSharp == 2)
      {
        loggedIn = atoi(tempNum);
      }
      else if(countSharp == 3)
      {
        lastCommandIsSpeed = atoi(tempNum);
      }
      else if(countSharp == 4)
      {
        accident = atoi(tempNum);
      }
      else if(countSharp == 5)
      {
        validStreet = atoi(tempNum);
      }
      else
      {
        replyLength = atoi(tempNum);
      }
      bzero(tempNum, sizeof(tempNum));
      j = 0;
      i++;
    }
    tempNum[j++] = serverPacket[i++];
  }
  i--;
  j = 0;
  while(serverPacket[i] != '#' && i < packetLength)
  {
    serverReply[j++] = serverPacket[i++];
  }
}
void updateStreet(char street[])
{
  bzero (command, MAX_COMMAND_LENG);
  bzero (serverReply, MAX_REPLY_LENG);
  strcpy(command, "street : ");
  strcat(command, street);
  makePacket(command, packet);
  errorCheck(errorFlag = send (socketDescriptor, &packet, MAX_COMMAND_LENG, 0), "send()");
}
void updateSpeed(char speed[])
{
  bzero (command, MAX_COMMAND_LENG);
  bzero (serverReply, MAX_REPLY_LENG);
  strcpy(command, "speed : ");
  strcat(command, speed);
  makePacket(command, packet);
  errorCheck(errorFlag = send (socketDescriptor, &packet, MAX_COMMAND_LENG, 0), "send()");
}
void sig_handler(int signum)
{
  timePassed = 1;
  alarm(60);
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
int isNumber(char num[])
{
  removeSpacesAndNewLine(num);
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
void* recvServerReply()
{
	bzero (serverReply, MAX_REPLY_LENG);
	errorCheck(errorFlag = recv (socketDescriptor, &serverPacket, MAX_REPLY_LENG, 0), "recv()");
	
	decodePacket();
	if(replyLength != strlen(serverReply))
	{
		printf("There was a temporary problem delivering your message. Please try again.");
	}
	else
	{
		printf ("%s\n", serverReply);
		if(accident)
		{
			printf(">> ");
			fflush(stdout);
		}
	}
	while(accident)
	{
		bzero (serverReply, MAX_REPLY_LENG);
		errorCheck(errorFlag = recv (socketDescriptor, &serverPacket, MAX_REPLY_LENG, 0), "recv()");
		
		decodePacket();
		if(replyLength != strlen(serverReply))
		{
			printf("There was a temporary problem delivering your message. Please try again.");
		}
		else
		{
			printf ("%s\n", serverReply);
      if(accident)
      {
        printf(">> ");
			  fflush(stdout);
      }
		}
	}
}
void makePacket(char commandMessage[], char packet[])
{
	char intToChar[10];
  int commLeng = strlen(commandMessage);
	sprintf(intToChar, "%d", commLeng);
	strcpy(packet, intToChar);
	strcat(packet, "#");
	strcat(packet, commandMessage);
	strcat(packet, "#");
}
