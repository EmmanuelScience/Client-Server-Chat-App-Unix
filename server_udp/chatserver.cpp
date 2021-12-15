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
struct client{
	int index;
	int sockID;
	struct sockaddr_in clientAddr;
	socklen_t len;
  char *username;
};
pthread_t server_t;
int clientCount = 0;
struct client Client[1024];
pthread_t thread[1024];
void connection_handler(void *);
char *passcode;
struct sockaddr_in server_address, client_address;
int server_port;
int clients[25];
int num_clients = 0;
int server_socket, client_socket,  opt = 1;
//listen for server input for listenlients functionality
void *server_input(void *nothing){
  while(1){
    char server_input[1024];
    scanf("%s",server_input);
    if(strcmp(server_input, "listclients") == 0){
      for(int i = 0;i < clientCount; i++){
        printf("client adress: %d, clientport : %d. ", 
          Client[i].clientAddr.sin_addr.s_addr, Client[i].clientAddr.sin_port );
      }
    }
    printf("\n");
  }
}




void connection_handler(void * ClientInfo){
  //function that handles connection for each client
  struct client *clientInfo  = (client *)ClientInfo;
  int index = clientInfo -> index;
  char data[1024];
  int read = recvfrom(server_socket, data, sizeof(data), 0, 
            (struct sockaddr *)&clientInfo->clientAddr, &clientInfo->len);
  data[read] = '\0';
  fprintf(stdout, "%.*s\n", 1024, data);
  for(int i = 0 ; i < clientCount ; i ++){
      if(i != index)
        sendto(server_socket, data, 1024,0,
            (struct sockaddr *)&Client[i].clientAddr , Client[i].len);
  }
}
int main(int argc, char *argv[]) {
   pthread_create(&server_t, NULL, server_input, NULL);
  //get arguements
  server_port = atoi(argv[3]);
  passcode = argv[5];

  //creates server socket
  if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
    printf("\n Error when creating  server socket\n");
		  return -1;
  }

  //sets server variables
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(server_port);
  server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Forcefully attaching socket to the port 8080
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

  //binds server
  if(bind(server_socket, (struct sockaddr*)&server_address,
    sizeof(server_address))<0){
      printf("\n %d\n", server_port);
      printf("\n Error when binding server socket\n");
		  return -1;
  }

  //after correctly creating the sockets it begins to accept connections
  printf("Server started on port %d. Accepting connections\n", server_port);
  while (1){
		char client_username[30];
    memset(client_username, 0, 30);
    char client_password[30];
    char buffer[1024];
    char buf[1024];
    int is_good = 1;
    
    socklen_t clientlen = sizeof(client_address);

    //get message
    bzero((char *) &client_address, sizeof(client_address));
    int valread = recvfrom(server_socket, buffer, sizeof(buffer), 0, 
    (struct sockaddr *)&client_address, &clientlen);
    int exists = 0;
    struct client my_client;

    //-----check if already in list of clients
    for(int i = 0; i < clientCount+1; i++){
      if(Client[i].clientAddr.sin_port == client_address.sin_port 
        && Client[i].clientAddr.sin_addr.s_addr == client_address.sin_addr.s_addr){
          exists = 1;
          my_client = Client[i];
          break;
        }
    }
    //---------------------------------------
    //if the client does not exist it checks the passwordd
    //and updates the client list
    if (exists == 0){
      int cond = 0, j = 0, count = 0;
      for (int i = 0; i < valread; i++){
        if(cond == 0 && buffer[i] != '+'){
          client_username[i] = buffer[i];
          count++;
        }
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
        sendto(server_socket, msg, readsize,0,
        (struct sockaddr *)&client_address, clientlen);
        
      }
      else{
        for(int i = 0; i < (int)strlen(passcode); i++){
          if (passcode[i] != client_password[i]){
            is_good = 0;
            char msg[64];
            int  readsize =  sprintf(msg, "Incorrect passcode");
            sendto(server_socket, msg, readsize,0,
            (struct sockaddr *)&client_address, clientlen);
            break;
          }
        }
      }
      if (is_good){
        char msg[64];
        sprintf(buf, "%.*s joined the chatroom",1024,  client_username);
        fprintf(stdout, "%.*s\n", 1024, buf);
        for(int i = 0 ; i < clientCount ; i ++){
            if(i != clientCount){
              sendto(server_socket, buf, 1024,0,
              (struct sockaddr *)&Client[i].clientAddr , Client[i].len);
            }
              
        }
        int  readsize =  sprintf(msg, "Connected to 127.0.0.1 on port %d",  server_port);
        sendto(server_socket, msg, readsize,0,
        (struct sockaddr *)&client_address, clientlen);
        Client[clientCount].clientAddr = client_address;
        Client[clientCount].username = client_username;
        Client[clientCount].len = clientlen;
        Client[clientCount].index = clientCount;
        clientCount++;
      }  
      
    }
    else{
      connection_handler((void *) &my_client); 
    }
  }
  for(int i = 0 ; i < clientCount ; i ++)
		pthread_join(thread[i],NULL);
}