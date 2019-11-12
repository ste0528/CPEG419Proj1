/* tcp_ client.c */ 
/* Programmed by Adarsh Sethi */
/* Sept. 19, 2019 */   
//files were edited using notepad++ and transferred over using WinSCP

#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset, memcpy, and strlen */
#include <netdb.h>          /* for struct hostent and gethostbyname */
#include <sys/socket.h>     /* for socket, connect, send, and recv */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */

#define LINE_SIZE 80

struct packetHeader{ 
  unsigned short count;
  unsigned short packetSequenceNumber;
};

int main(void) {
   int sock_client;  /* Socket used by client */
   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   struct hostent * server_hp;      /* Structure to store server's IP
                                        address */
   char server_hostname[LINE_SIZE]; /* Server's hostname */
   unsigned short server_port;  /* Port number used by server (remote port) */

   unsigned int msg_len;  /* length of message */                      
   int bytes_sent, bytes_recd; /* number of bytes sent or received */
   FILE *output; // create output file
   output = fopen("out.txt","w"); //open the output file
   char line[LINE_SIZE]; // line buffer
   struct packetHeader packets; // header
   char inputFile[LINE_SIZE]; // file name to send
   int totalByteCount = 0; // total bytes received

   /* open a socket */
   if ((sock_client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Client: can't open stream socket");
      exit(1);
   }

   /* Note: there is no need to initialize local client address information 
            unless you want to specify a specific local port
            (in which case, do it the same way as in udpclient.c).
            The local address initialization and binding is done automatically
            when the connect function is called later, if the socket has not
            already been bound. */
   /* initialize server address information */

   printf("Enter hostname of server: ");
   scanf("%s", server_hostname);
   if ((server_hp = gethostbyname(server_hostname)) == NULL) {
      perror("Client: invalid server hostname");
      close(sock_client);
      exit(1);
   }

   printf("Enter port number for server: ");
   scanf("%hu", &server_port);

   /* Clear server address structure and initialize with server address */
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   memcpy((char *)&server_addr.sin_addr, server_hp->h_addr,
                                    server_hp->h_length);
   server_addr.sin_port = htons(server_port);

    /* connect to the server */
   if (connect(sock_client, (struct sockaddr *) &server_addr, 
                                    sizeof (server_addr)) < 0) {
      perror("Client: can't connect to server");
      close(sock_client);
      exit(1);
   }
   
   //Have the user enter in the file name that they would like to transfer
   printf("Please enter the name of the file to transfer: ");
   scanf("%s", inputFile);
   
   //Make the packetHeader for the input file name
   packets.count = htons(packets.count);
   packets.packetSequenceNumber = htons(0);
   msg_len = sizeof(struct packetHeader);
   
   //Send the file name that will be transferred
   bytes_sent = send(sock_client, &packets, msg_len, 0);
   bytes_sent = send(sock_client, inputFile, strlen(inputFile) + 1, 0);
   
   //Get the line from the server and print a message stating how many bytes were received 
   bytes_recd = recv(sock_client, &packets, sizeof(struct packetHeader), 0);
   packets.count = ntohs(packets.count);
   packets.packetSequenceNumber = ntohs(packets.packetSequenceNumber);
   bytes_recd = recv(sock_client, line, packets.count, 0);
   totalByteCount = totalByteCount + bytes_recd;
   printf("Packet %i received with %i data bytes\n", packets.count, packets.packetSequenceNumber);

   //Continue to get lines until it reaches the end of file packet
   while(packets.count != 0) {
     fputs(line, output); 
     bytes_recd = recv(sock_client, &packets, sizeof(struct packetHeader), 0);
     packets.count = ntohs(packets.count);
     packets.packetSequenceNumber = ntohs(packets.packetSequenceNumber);

     //When the count is equal to 0, we know that it has reached the end of the transmission
     if(packets.count == 0) {
       printf("End of Transmission Packet received with sequence number %i with %i data bytes\n", packets.packetSequenceNumber, packets.count);	       
     }
	 
	 //Receive bytes and display packet number and number of bytes
	 bytes_recd = recv(sock_client, line, packets.count, 0);
     totalByteCount = totalByteCount + bytes_recd;
     printf("Packet %i received with %i data bytes\n", packets.packetSequenceNumber, packets.count);
   }

   //Print the final message with the total values
   printf("Total data packets received: %i\n", packets.packetSequenceNumber - 1);
   printf("Total bytes recieved: %i\n", totalByteCount);

   // close the output file and the socket 
   fclose(output);
   close (sock_client);
}