#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#define BufferLength 4096

int main()
{

/* Variable and structure definitions. */

int sd, sd2, rc, length = sizeof(int);
int totalcnt = 0, on = 1,i,dataSize;
char temp;
char buffer[BufferLength];
int csz = sizeof(struct sockaddr_in);
struct sockaddr_in serveraddr;
struct sockaddr_in their_addr;
fd_set read_fd;
struct timeval timeout;
timeout.tv_sec = 15;
timeout.tv_usec = 0;

/* Get a socket descriptor */

if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Error while socket creation.\n");
		exit (-1);
	}
else
printf("Socket creation successful.\n");

if((rc = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))) < 0)
	{
		perror("Socket error.");
		close(sd);
		exit (-1);
	}

/* bind to an address */

memset(&serveraddr, 0x00, sizeof(struct sockaddr_in));
serveraddr.sin_family = AF_INET;
serveraddr.sin_port=0;
serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

/* connect to any client */

if((rc = bind(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr))) < 0)
	{
		perror("Error while binding.\n");
		close(sd);
		exit(-1);
	}
else
printf("Bind Successful\n");

/*generate port number dynamically*/

getsockname( sd, (struct sockaddr *)&serveraddr,&csz);
printf("Port Number where server bind: %d\n",ntohs(serveraddr.sin_port));

if((rc = listen(sd, 10)) < 0)
	{
		perror("Server Listen error.\n");
		close(sd);
		exit (-1);
	}
else
printf("Server-Ready for client connection...\n");

/* accept() the incoming connection request. */

int sin_size = sizeof(struct sockaddr_in);
if((sd2 = accept(sd, (struct sockaddr *)&their_addr, &sin_size)) < 0)
	{
		perror("Server accept error.\n");
		close(sd);
		exit (-1);
	}
else
printf("Server accept is successful.\n");

/* Read data from the client. */

while(1)
	{

		while(1)
			{
				totalcnt = 0;
				while(totalcnt < BufferLength)
					{
						/* read() from client */
						
						ServerRead:
						rc = read(sd2, &buffer[totalcnt], (BufferLength - totalcnt));
						if(rc < 0)
							{
								perror("Server-read() error");
								close(sd);
								close(sd2);
								exit (-1);
							}
						else if (rc == 0)
								{
									printf("Client program has issued a close()\n");
									close(sd);
									close(sd2);
									exit(-1);
								}
						else
							{
								totalcnt += rc;
							}
						printf("Client says: %s", buffer);
						break;
					}
				dataSize=strlen(buffer);
				if (strcmp(buffer,"end\n")==0) 
					{
						goto ServerWrite;
					}
			}

/* write() some bytes of string, */

ServerWrite:

while(1)
	{
		printf("Server says:");
		i=0;
		memset(buffer, 0, BufferLength);
		while(1)
		{
		buffer[i]= getchar();
		if(buffer[i]=='\n') break;
		i++;
		}
		rc = write(sd2, buffer, sizeof(buffer));
		if(rc != sizeof(buffer))
			{
				perror("Server-write() error");
				
				/* Get the error number. */

				rc = getsockopt(sd2, SOL_SOCKET, SO_ERROR, &temp, &length);
				if(rc == 0)
					{

						/* Print out the asynchronously  received error. */

						errno = temp;
						perror("SO_ERROR was: ");
					}
				close(sd);
				close(sd2);
				exit(-1);
			}
		dataSize=strlen(buffer);
		if (strcmp(buffer,"end\n")==0) 
		break;
	}
	
	}
/* Close the connection to the client and */

close(sd2);
close(sd);
exit(0);
return 0;
}