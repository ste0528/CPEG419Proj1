/* udp_server.c */
/* Programmed by Adarsh Sethi */
/* Sept. 19, 2019 */

#include <ctype.h>          /* for toupper */
#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset */
#include <sys/socket.h>     /* for socket, sendto, and recvfrom */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */
#include <time.h>           //for srand()

#define STRING_SIZE 1024
#define LINE_SIZE 80

/* SERV_UDP_PORT is the port number on which the server listens for
   incoming messages from clients. You should change this to a different
   number to prevent conflicts with others in the class. */

#define SERV_UDP_PORT 5100
int simulateLoss(void);
float plr; //packet loss ratio number

struct packetHeader {
  unsigned short count;
  unsigned short packetSequenceNumber;
};

int main(void) {

   int sock_server;  /* Socket on which server listens to clients */

   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   unsigned short server_port;  /* Port number used by server (local port) */

   struct sockaddr_in client_addr;  /* Internet address structure that
                                        stores client address */
   unsigned int client_addr_len;  /* Length of client address structure */

   char sentence[STRING_SIZE];  /* receive message */
   char modifiedSentence[STRING_SIZE]; /* send message */
   unsigned int msg_len;  /* length of message */
   int bytes_sent, bytes_recd; /* number of bytes sent or received */
   unsigned int i;  /* temporary loop variable */

   //added variables
   int timeout;                   //timeout value
   FILE *inputFile;               //make the file that will be read from
   char lineInput[LINE_SIZE];     //character array made to store one line from the text file
   struct packetHeader packets;   //make a structure of types packetHeader named packets
   int totalByteCount = 0;        //keep track of the total number of bytes transferred

   srand(time(0));                //seed for rand which is called later in the main

   /* open a socket */

   if ((sock_server = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
      perror("Server: can't open datagram socket\n");
      exit(1);
   }

   /* initialize server address information */

   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = htonl (INADDR_ANY);  /* This allows choice of
                                        any host interface, if more than one
                                        are present */
   server_port = SERV_UDP_PORT; /* Server will listen on this port */
   server_addr.sin_port = htons(server_port);

   /* bind the socket to the local server port */

   if (bind(sock_server, (struct sockaddr *) &server_addr,
                                    sizeof (server_addr)) < 0) {
      perror("Server: can't bind to local address\n");
      close(sock_server);
      exit(1);
   }

   /* wait for incoming messages in an indefinite loop */
   printf("Please enter a timeout value (1-10):\n");
   scanf("%d", &timeout);
   printf("Please enter a packet loss ratio (0-1):\n");
   scanf("%lf", &plr);

   printf("Waiting for incoming messages on port %hu\n\n", server_port);
   client_addr_len = sizeof (client_addr);

   for (;;) {
      bytes_recd = recvfrom(sock_server, &sentence, STRING_SIZE, 0,
                     (struct sockaddr *) &client_addr, &client_addr_len);
      printf("Received Sentence is: %s\n     with length %d\n\n", sentence, bytes_recd);

      /* prepare the message to send */

      msg_len = bytes_recd;
      for (i=0; i<msg_len; i++)
         modifiedSentence[i] = toupper (sentence[i]);

      /* send message */
      if(!simulateLoss()){ //need to parse plr from message sent
        bytes_sent = sendto(sock_server, modifiedSentence, msg_len, 0,
                 (struct sockaddr*) &client_addr, client_addr_len);
      }
   }

  //  while(fgets(lineInput, sizeof(lineInput), inputFile) != NULL){
  //    packets.count = htons(strlen(lineInput) + 1);
  //    packets.packetSequenceNumber = ntohs(packets.packetSequenceNumber) + 1;
  //    packets.packetSequenceNumber = htons(packets.packetSequenceNumber);
  //
  //  //send the packet header and the line that was read in. Then display the packet number and the number of data bytes
  //    bytes_sent = send(sock_connection, &packets, sizeof(struct packetHeader), 0);
  //  bytes_sent = send(sock_connection, lineInput, ntohs(packets.count), 0);
  //    totalByteCount = totalByteCount + bytes_sent;
  //    printf("Packet %hu transmitted with %hu data bytes\n", ntohs(packets.packetSequenceNumber), ntohs(packets.count));
  // }
}

//Gotta figure out how to get the alr from the client
int simulateLoss(void){
    float generatednumber = (rand() % 1);
    if(generatednumber < plr){
      return 1;
    }
    else{
      return 0;
    }
}
