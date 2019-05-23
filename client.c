#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define PORT 2222
void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];


    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("Napaka pri kreiranju socketa");
    }

    server = gethostbyname("127.0.0.1");

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(PORT);

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){ 
        error("Napaka pri povezavi");
    }

    printf("Vnesite sporocilo: ");
    bzero(buffer,256);//nastavimo vse znake na 0 oz. zbrisemo
    fgets(buffer,255,stdin);


    if (write(sockfd,buffer,strlen(buffer)) < 0){ 
         error("Napaka pri posiljanju na streznik");
    }

    bzero(buffer,256);//nastavimo vse znake na 0 oz. zbrisemo

    if (read(sockfd,buffer,255) < 0){ 
         error("Napaka pri branju sporocila z streznika");
    }

    printf("%s\n",buffer);

    close(sockfd);

    return 0;
}