#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT       8888		        /* Port number as an integer */
#define IP_ADDRESS "192.168.0.8"	/* IP Address as a string */

#define BUFSIZE 8196

char *command = "GET /index.html HTTP/1.0 \r\n\r\n" ;

void pexit(char *msg)
{
    perror(msg);
    exit(1);
}

int main()
{
    int i, sock_fd;
    char buffer[BUFSIZE];
    static struct sockaddr_in server_addr;

    printf("Trying to connect to %s and port %d\n", IP_ADDRESS, PORT);
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) <0)
        pexit("socket() failed");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    server_addr.sin_port = htons(PORT);

    /* Connects to the web server socket */
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <0)
        pexit("connect() failed");

    /* Sends the GET request to the server using sock_fd */
    printf("Send bytes=%d %s\n", (int)strlen(command), command);
    write(sock_fd, command, strlen(command));

    /* Displays the requested raw HTML file as responded from the server */
    while ((i = read(sock_fd, buffer, BUFSIZE)) > 0)
        write(1, buffer, i);
}
