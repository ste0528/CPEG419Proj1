/* tcpserver.c */
/* Programmed by Adarsh Sethi */
/* Sept. 19, 2019 */    
//files were edited using notepad++ and transferred over using WinSCP  

#include <ctype.h>          /* for toupper */
#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset */
#include <sys/socket.h>     /* for socket, bind, listen, accept */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */

#define LINE_SIZE 80   

/* SERV_TCP_PORT is the port number on which the server listens for
   incoming requests from clients. You should change this to a different
   number to prevent conflicts with others in the class. */

#define SERV_TCP_PORT  11235 

//Structure that is used to hold header
struct packetHeader {
  unsigned short count;
  unsigned short packetSequenceNumber;
};

int main(void) {
   int sock_server;  /* Socket on which server listens to clients */
   int sock_connection;  /* Socket on which server exchanges data with client */
   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   unsigned int server_addr_len;  /* Length of server address structure */
   unsigned short server_port;  /* Port number used by server (local port) */
   struct sockaddr_in client_addr;  /* Internet address structure that
                                        stores client address */
   unsigned int client_addr_len;  /* Length of client address structure */
   unsigned int msg_len;  /* length of message */
   int bytes_sent, bytes_recd; /* number of bytes sent or received */
   unsigned int i;  /* temporary loop variable */

   //Added variables
   FILE *inputFile; //make the file that will be read from 
   char lineInput[LINE_SIZE];//character array made to store one line from the text file
   struct packetHeader packets; //make a structure of types packetHeader named packets
   int totalByteCount = 0; //keep track of the total number of bytes transferred 

   /* open a socket */
   if ((sock_server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Server: can't open stream socket");
      exit(1);                                                
   }

   /* initialize server address information */
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = htonl (INADDR_ANY);  /* This allows choice of
                                        any host interface, if more than one
                                        are present */ 
   server_port = SERV_TCP_PORT; /* Server will listen on this port */
   server_addr.sin_port = htons(server_port);

   /* bind the socket to the local server port */
   if (bind(sock_server, (struct sockaddr *) &server_addr,
                                    sizeof (server_addr)) < 0) {
      perror("Server: can't bind to local address");
      close(sock_server);
      exit(1);
   }                     

   /* listen for incoming requests from clients */
   if (listen(sock_server, 50) < 0) {    /* 50 is the max number of pending */
      perror("Server: error on listen"); /* requests that will be queued */
      close(sock_server);
      exit(1);
   }
   printf("I am here to listen ... on port %hu\n\n", server_port);
   client_addr_len = sizeof (client_addr);

   /* wait for incoming connection requests in an indefinite loop */
    sock_connection = accept(sock_server, (struct sockaddr *) &client_addr, 
                                         &client_addr_len);
                     /* The accept function blocks the server until a
                        connection request comes from a client */
    if (sock_connection < 0) {
       perror("Server: accept() error\n");         
       close(sock_server);
       exit(1);
    }
 
    //Receive a message
	bytes_recd = recv(sock_connection, &packets, sizeof(struct packetHeader), 0);
    packets.count = ntohs(packets.count);
    packets.packetSequenceNumber = ntohs(packets.packetSequenceNumber);

	//Get the input text file name and open it so the program can read from it 
    char readFrom[packets.count + 1];
    bytes_recd = recv(sock_connection, readFrom, packets.count, 0); 
	inputFile = fopen(readFrom, "r");
    
    // read in the file specified by the user and send it over line by line until the it reaches the end of the input text file
    while(fgets(lineInput, sizeof(lineInput), inputFile) != NULL){
    packets.count = htons(strlen(lineInput) + 1); 
    packets.packetSequenceNumber = ntohs(packets.packetSequenceNumber) + 1; 
    packets.packetSequenceNumber = htons(packets.packetSequenceNumber);
	
	//send the packet header and the line that was read in. Then display the packet number and the number of data bytes
    bytes_sent = send(sock_connection, &packets, sizeof(struct packetHeader), 0);
	bytes_sent = send(sock_connection, lineInput, ntohs(packets.count), 0);
    totalByteCount = totalByteCount + bytes_sent;
    printf("Packet %hu transmitted with %hu data bytes\n", ntohs(packets.packetSequenceNumber), ntohs(packets.count));
   }

  //Send end of transmission packet
  packets.count = htons(0); 
  packets.packetSequenceNumber = ntohs(packets.packetSequenceNumber) + 1;
  packets.packetSequenceNumber = htons(packets.packetSequenceNumber);
  bytes_sent = send(sock_connection, &packets, sizeof(struct packetHeader), 0);
  printf("End of Transmission Packet with sequence number %hu transmitted with %hu data bytes\n", ntohs(packets.packetSequenceNumber), ntohs(packets.count));
  printf("Total data packets transmitted: %hu\n", ntohs(packets.packetSequenceNumber) - 1);
  printf("Total data bytes transmitted: %i\n", totalByteCount);
  
  //Close the input file that was specified by the user and the socket 
  fclose(inputFile);
  close(sock_connection);
}