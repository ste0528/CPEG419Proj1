/* tcp_ client.c */ 
/* Programmed by Adarsh Sethi */
/* Sept. 19, 2019 */     

#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset, memcpy, and strlen */
#include <netdb.h>          /* for struct hostent and gethostbyname */
#include <sys/socket.h>     /* for socket, connect, send, and recv */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */

#define STRING_SIZE 1024

int main(void) {

   int sock_client;  /* Socket used by client */

   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   struct hostent * server_hp;      /* Structure to store server's IP
                                        address */
   char server_hostname[STRING_SIZE]; /* Server's hostname */
   unsigned short server_port;  /* Port number used by server (remote port) */

   char sentence[STRING_SIZE];  /* send message */
   char modifiedSentence[STRING_SIZE]; /* receive message */
   unsigned int msg_len;  /* length of message */                      
   int bytes_sent, bytes_recd; /* number of bytes sent or received */
  
   FILE * file;
   int charCount;
   char ch;
   int lineCount = 0;
   char text[STRING_SIZE];
   FILE *fp;
   fp = fopen("output.txt","w");

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
  
   /* user interface */
   file = fopen("test2.txt", "r");
   printf("before while loop\n");
   
   
   while ((ch = fgetc(file)) != EOF){
	   if(ch == '\n' || ch == '\0'){
	   lineCount++;
	}
    }
   printf("Total new lines: %i", lineCount);    
   fclose(file);
   file = fopen("test2.txt", "r");  
   fgets(sentence, STRING_SIZE, file);  
   msg_len = strlen(sentence) + 1;
   fputs(sentence, fp);

   /* send message */   
   bytes_sent = send(sock_client, sentence, msg_len, 0);
   
   
   /* get response from server */
   bytes_recd = recv(sock_client, modifiedSentence, STRING_SIZE, 0); 
         
   printf("\nThe response from server is:\n");
   printf("%s\n\n", modifiedSentence);
      
   /* close the socket */

   fclose(file);
   fclose(fp);
   close (sock_client);
}
