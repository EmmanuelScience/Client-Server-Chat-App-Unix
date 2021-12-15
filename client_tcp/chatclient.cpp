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

char user[1024];
void * recieve_msg(void * sockID){
	//waits for message from the server and prints it
	int clientSocket = *((int *) sockID);
	while(1){
		char data[1024];
		int read = recv(clientSocket,data,1024,0);
		data[read] = '\0';
		printf("%s\n",data);

	}

}

void send_msg(int clientSocket)
//when a message is recieved it's sent to the server
{
    while(1){
		char input[1024];
		char firstMsg[1024];
		char fullmsg[1024];
		char usernameMsg[1024];
		memset(usernameMsg, 0, 1024);
		strcpy(usernameMsg, user);
		scanf("%s",firstMsg);
		sprintf(input, "SEND");
		if (strcmp(firstMsg, ":Exit") ==0){
			memset(input, 0, 1024);
			scanf("%[^\n]s",fullmsg);
			sprintf(input, ":Exit");
			send(clientSocket,input,1024,0);
			strcat(usernameMsg, " left the chatroom");
			send(clientSocket,usernameMsg,1024,0);
			close(clientSocket);
		} 
		else if(strcmp(firstMsg, ":)") ==0){
			memset(input, 0, 1024);
			scanf("%[^\n]s",fullmsg);
			sprintf(input, ":Exit");
			send(clientSocket,input,1024,0);
			strcat(usernameMsg, ": [feeling happy]");
			send(clientSocket,usernameMsg,1024,0);
		}
		else if(strcmp(firstMsg, ":+1hr") ==0){
			memset(input, 0, 1024);
			scanf("%[^\n]s",fullmsg);
			sprintf(input, ":Exit");
			send(clientSocket,input,1024,0);
			time_t now;
 			time(&now);
      		now += 60 * 60;
			strcat(usernameMsg, ": ");
			strcat(usernameMsg,ctime(&now)) ;
			send(clientSocket,usernameMsg,strlen(usernameMsg)-1,0);
		}
		else if(strcmp(firstMsg, ":(") ==0){
			memset(input, 0, 1024);
			scanf("%[^\n]s",fullmsg);
			sprintf(input, ":Exit");
			send(clientSocket,input,1024,0);
			strcat(usernameMsg, ": [feeling sad]");
			send(clientSocket,usernameMsg,1024,0);
		}
		else if(strcmp(firstMsg, ":mytime") ==0){
			memset(input, 0, 1024);
			scanf("%[^\n]s",fullmsg);
			sprintf(input, ":Exit");
			send(clientSocket,input,1024,0);
			time_t now;
 			time(&now);
			strcat(usernameMsg, ": ");
			strcat(usernameMsg,ctime(&now)) ;
			send(clientSocket,usernameMsg,strlen(usernameMsg)-1,0);
		}
		else{
			send(clientSocket,input,1024,0);
			memset(fullmsg, 0, 1024);
			scanf("%[^\n]s",fullmsg);
			strcat(firstMsg, fullmsg);
			strcat(usernameMsg, ": ");
			strcat(usernameMsg, firstMsg);
			send(clientSocket,usernameMsg,1024,0);
		}
	}
}
int main(int argc, char *argv[]) {
	//Recieving ipnut variables 
	int server_port = atoi(argv[5]);
	char* host_name = argv[3];
	char* username = argv[7];
	memset(user, 0, 1024);
	strcpy(user, username);
	char* password = argv[9];

	//prepare and sends username and password
	char data_to_send[64];
	memset(data_to_send, 0, 64);
	strcpy(data_to_send, username);
	strcat(data_to_send, "+");
	strcat(data_to_send, password);

	//declare sockets variables
	int client_socket;
	struct sockaddr_in server_address;

	//creates client Socket
	if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("\n Error when Creating Socket\n");
		return -1;
	}
	

	//Sets server's address Variables
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(server_port);
	server_address.sin_addr.s_addr = inet_addr(host_name);

	//Connects TCP client socket to a server
	if (connect (client_socket, (struct sockaddr*)&server_address,
		sizeof(server_address))<0){
			printf("\n Error when Connecting Socket to server\n");
			return -1;
		}
	

	//sends username and password to serever
	if (send(client_socket, data_to_send, strlen(data_to_send), 0)<0){
		printf("\n Error when sending username \n");
		return -1;
	}
	char password_check[64];
	int count;
	//recieves password confirmation
	count = recv(client_socket, password_check, 512, 0);
	fprintf(stdout, "%.*s\n", count, password_check);

	//close program if password is incorrect
	if(strcmp(password_check, "Incorrect passcode") == 0){
		return 0;
	}
	//calls thread that recieves messages
	pthread_t thread;
	pthread_create(&thread, NULL, recieve_msg, (void *) &client_socket );
	send_msg(client_socket);
}