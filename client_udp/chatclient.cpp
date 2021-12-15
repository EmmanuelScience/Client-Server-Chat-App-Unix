/**
 * Author: Emmanuel Onyekachukwu Irabor
 * GTID: 903745596
 * GT Email: eirabor3@gatech.edu
 */
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <string.h>
#include <cstdio>
#include <pthread.h>
#include<arpa/inet.h>
#include <time.h>

int client_socket, valread;
struct sockaddr_in server_address;
socklen_t serverlen = sizeof(server_address);
char user[1024];

void * recieve_msg(void * sockID){
	while(1){
		char data[1024];
		int read = recvfrom(client_socket, data, sizeof(data), 0, 
    	(struct sockaddr *)&server_address, &serverlen);
		data[read] = '\0';
		printf("%s\n",data);
	}
}


void send_msg(int clientSocket)
{
    while(1){
		//while loop that waits for the client to type a message
		//and sends it
		char input[1024];
		char fake[1024];
		char work[1024];
		char usernameMsg[1024];
		memset(usernameMsg, 0, 1024);
		strcpy(usernameMsg, user);
		scanf("%s",fake);
		sprintf(input, "SEND");
		if (strcmp(fake, ":Exit") ==0){
			memset(input, 0, 1024);
			scanf("%[^\n]s",work);
			sprintf(input, ":Exit");
			sendto(client_socket, input, strlen(input),0, 
      			(struct sockaddr *)&server_address, serverlen);
			strcat(usernameMsg, " left the chatroom");
			sendto(client_socket, usernameMsg, strlen(usernameMsg),0, 
  				(struct sockaddr *)&server_address, serverlen);
			close(clientSocket);
		} 
		else if(strcmp(fake, ":)") ==0){
			memset(input, 0, 1024);
			scanf("%[^\n]s",work);
			sprintf(input, ":Exit");
			sendto(client_socket, input, strlen(input),0, 
  				(struct sockaddr *)&server_address, serverlen);
			strcat(usernameMsg, ": [feeling happy]");
			sendto(client_socket, usernameMsg, strlen(usernameMsg),0, 
  				(struct sockaddr *)&server_address, serverlen);
		}
		else if(strcmp(fake, ":(") ==0){
			memset(input, 0, 1024);
			scanf("%[^\n]s",work);
			sprintf(input, ":Exit");
			sendto(client_socket, input, strlen(input),0, 
  				(struct sockaddr *)&server_address, serverlen);
			strcat(usernameMsg, ": [feeling sad]");
			sendto(client_socket, usernameMsg, strlen(usernameMsg),0, 
  				(struct sockaddr *)&server_address, serverlen);
		}
		else if(strcmp(fake, ":+1hr") ==0){
			memset(input, 0, 1024);
			scanf("%[^\n]s",work);
			sprintf(input, ":Exit");
			sendto(client_socket, input, strlen(input),0, 
  				(struct sockaddr *)&server_address, serverlen);
			time_t now;
 			time(&now);
      		now += 60 * 60;
			strcat(usernameMsg, ": ");
			strcat(usernameMsg,ctime(&now)) ;
			sendto(client_socket, usernameMsg, strlen(usernameMsg)-1,0, 
  				(struct sockaddr *)&server_address, serverlen);
		}
    	else if(strcmp(fake, ":mytime") ==0){
			memset(input, 0, 1024);
			scanf("%[^\n]s",work);
			sprintf(input, ":Exit");
			sendto(client_socket, input, strlen(input),0, 
  				(struct sockaddr *)&server_address, serverlen);
			time_t now;
 			time(&now);
			strcat(usernameMsg, ": ");
			strcat(usernameMsg,ctime(&now)) ;
			sendto(client_socket, usernameMsg, strlen(usernameMsg)-1,0, 
  				(struct sockaddr *)&server_address, serverlen);
		}
		else{
			sendto(client_socket, input, strlen(input),0, 
  				(struct sockaddr *)&server_address, serverlen);
			memset(work, 0, 1024);
			scanf("%[^\n]s",work);
			strcat(fake, work);
			strcat(usernameMsg, ": ");
			strcat(usernameMsg, fake);
			sendto(client_socket, usernameMsg, strlen(usernameMsg),0, 
  				(struct sockaddr *)&server_address, serverlen);
		}
	}
}

int main(int argc, char *argv[]) {
	//checks input values
	if (argc != 10){
		printf("\n Wrong input values\n");
		return -1;
	}

	//Recieving ipnut variables 
	int server_port = atoi(argv[5]);
	char* host_name = argv[3];
	char* username = argv[7];
	memset(user, 0, 1024);
	strcpy(user, username);
	char* password = argv[9];

	char data_to_send[64];
	memset(data_to_send, 0, 64);
	strcpy(data_to_send, username);
	strcat(data_to_send, "+");
	strcat(data_to_send, password);

	//creates client Socket
	if((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		printf("\n Error when Creating Socket\n");
		return -1;
	}
	
	//Sets server's address Variables
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(server_port);
	server_address.sin_addr.s_addr = inet_addr(host_name);

	//sends username and password to serever
	if (sendto(client_socket, data_to_send, strlen(data_to_send),0, 
  		(struct sockaddr *)&server_address, serverlen)<0){
		printf("\n Error when sending username \n");
		return -1;
	}
	char password_check[64];
	int count;
  	count = recvfrom(client_socket, password_check, sizeof(password_check), 0, 
    	(struct sockaddr *)&server_address, &serverlen);
	fprintf(stdout, "%.*s\n", count, password_check);
	if(strcmp(password_check, "Incorrect passcode") == 0){
		return 0;
	}
	pthread_t thread;
	pthread_create(&thread, NULL, recieve_msg, (void *) &client_socket );
	send_msg(client_socket);
}