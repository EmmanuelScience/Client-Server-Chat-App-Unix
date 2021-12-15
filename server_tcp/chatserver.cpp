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
#include <pthread.h>
#include<arpa/inet.h>
int clientCount = 0;
struct client{

	int index;
	int sockID;
	struct sockaddr_in clientAddr;
	socklen_t len;
  char *username;

};

struct client Client[1024];
pthread_t thread[1024];
pthread_t server_t;

void *connection_handler(void *);
void *server_input(void *);
char *passcode;
struct sockaddr_in server_address, client_address;
int server_port;
int clients[25];
int num_clients = 0;

pthread_mutex_t lock;

//listen for server input for listenlients functionality
void *server_input(void *nothing){
  while(1){
    char server_input[1024];
    scanf("%s",server_input);
    if(strcmp(server_input, "listclients") == 0){
      for(int i = 0;i < clientCount; i++){
        printf("client with socketfd: %d ", Client[i].sockID);
      }
    }
    printf("\n");
  }
}
int main(int argc, char *argv[]) {
  //thread that keeps listening for an input
  pthread_create(&server_t, NULL, server_input, NULL);
  int server_socket,  opt = 1;

  //get arguements
  server_port = atoi(argv[3]);
  passcode = argv[5];

  //creates server socket
  if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    printf("\n Error when creating  server socket\n");
		  return -1;
  }

  //sets server variables
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(server_port);
  server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Forcefully attaching socket to a port
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
      &opt, sizeof(opt)))
    {
        printf("setsockopt");
        return -1;
    }

  //binds server to port
  if(bind(server_socket, (struct sockaddr*)&server_address,
    sizeof(server_address))<0){
      printf("\n Error when binding server socket\n");
		  return -1;
  }

  //listening to socket
  if(listen(server_socket, 1)< 0){
    printf("\n Error when listening to Socket\n");
		return -1;
  }
  
  //server has been created successfully, now we wait for connections
  printf("Server started on port %d. Accepting connections\n", server_port);


  while (1){
    //infinite while loop that listens ffor tcp  connections
    Client[clientCount].sockID = accept(server_socket, 
    (struct sockaddr*) &Client[clientCount].clientAddr, &Client[clientCount].len);

    //gets the username and password and checks the correctness of the username
    char client_username[30];
    memset(client_username, 0, 30);
    char client_password[30];
    char buffer[1024];
    char buf[1024];
    int is_good = 1;

    //get message
    int valread = recv(Client[clientCount].sockID,buffer,1024,0);
    int cond = 0, j = 0, count = 0;
    for (int i = 0; i < valread; i++){
      if(cond == 0 && buffer[i] != '+'){
        client_username[i] = buffer[i];
        count++;
      }    //fflush( stdout );
      else if (cond == 1){
        client_password[j] = buffer[i];
        j++;
      }
      if (buffer[i] == '+'){
        cond = 1;
      }
    }
    if (j != (int)strlen(passcode)){
      is_good = 0;
      char msg[64];
      int  readsize =  sprintf(msg, "Incorrect passcode");
      write(Client[clientCount].sockID , msg, readsize);
    }
    else{
      for(int i = 0; i < (int)strlen(passcode); i++){
        if (passcode[i] != client_password[i]){
          is_good = 0;
          char msg[64];
          int  readsize =  sprintf(msg, "Incorrect passcode");
          write(Client[clientCount].sockID , msg, readsize);
          break;
        }
      }  
    }

    //if the password is correct the is_good variable will be set to true
    //then we add the client to the chatroom
    if (is_good){
      char msg[64];
      sprintf(buf, "%.*s joined the chatroom",1024,  client_username);
      fprintf(stdout, "%.*s\n", 1024, buf);
      for(int i = 0 ; i < clientCount ; i ++){
			  send(Client[i].sockID,buf,1024,0);
			}
    
      int  readsize =  sprintf(msg, "Connected to 127.0.0.1 on port %d",  server_port);
      write(Client[clientCount].sockID , msg, readsize);
      
      Client[clientCount].index = clientCount;
      pthread_create(&thread[clientCount], NULL, connection_handler, (void *) &Client[clientCount]);
      clientCount ++;
    }
  }
  
  for(int i = 0 ; i < clientCount ; i ++)
		pthread_join(thread[i],NULL);

}

void *connection_handler(void * ClientInfo){
  //thread that hanmdles eaxh tcp connection
  struct client *clientInfo  = (client *)ClientInfo;
  int index = clientInfo -> index;
  int clientSocket = clientInfo -> sockID;
  while(1){
    char data[1024];
    char test[1024];
		int read = recv(clientSocket,data,1024,0);
		data[read] = '\0';
    memset(test, 0, 1024);
    read = recv(clientSocket,test,1024,0);
    test[read] = '\0';

    //prints out recieved message to the server output
    fprintf(stdout, "%.*s\n", 1024, test);

    //sends recieved message to all the clients
    for(int i = 0 ; i < clientCount ; i ++){
      if(i != index)
        send(Client[i].sockID,test,1024,0);
    }
  }
}