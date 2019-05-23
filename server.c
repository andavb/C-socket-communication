#include <stdio.h>
#include <sys/utsname.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#define PORT 2222
void error2(char *sporocilo, int sock)//izven zanke, zapre socketfd
{
    perror(sporocilo);
    close(sock);
    exit(1);
}
void error2(char *sporocilo)//v zanki izpise samo sporocilo
{
    perror(sporocilo);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, clilen, i, j, pomoc, rokada = 0;
    char buffer[256], path[PATH_MAX], skupaj[500];
    char str = '0', pomozen[10];
    struct sockaddr_in serv_addr, cli_addr, * pV4Addr;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    struct utsname uname_pointer;


     sockfd = socket(AF_INET, SOCK_STREAM, 0);//(AF_INET - uporabi ipv4)(SOCK_STREAM - tip povezave)ustvari vtičnico 
     if (sockfd == -1) {
        error("Napaka pri kreiranju socketa", sockfd);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(PORT);


    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){// bind v OS rezervira stvari za vticnico, preveri ali so vrata prosta
        error("Napaka pri bind\n", sockfd);
    }

    if(listen(sockfd,4) == -1){//Aktivira vticnico, zdaj je omogoceno da se kdo poveže
        error("Napaka pri poslusanju\n", sockfd);
    }
    printf("Poslusam na naslovu [%s:%d]\n", inet_ntoa(serv_addr.sin_addr), (int) ntohs(serv_addr.sin_port));

    clilen = sizeof(cli_addr);

    while(1){
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);// accept vzpostavi povezavo 
        if (newsockfd < 0){ 
            error2("Napaka pri acceptu\n");
        }

        printf("\nOdjemalec se je povezal [%s:%d]\n", inet_ntoa(cli_addr.sin_addr), (int) ntohs(cli_addr.sin_port));

        bzero(buffer,256);//nastavimo vse znake na 0 oz. zbrisemo
        bzero(skupaj, 256);

        if (read(newsockfd,buffer,255) < 0){// branje s podatkovnega toka
            error2("Napaka pri branju z vtičnice\n");
        }

        printf("Dobil sem sporocilo: %s\n",buffer);

        str = buffer[0];

        if(str == 'A'){
            strcpy(skupaj, "Pozdravljen [");
            strcat(skupaj, inet_ntoa(cli_addr.sin_addr));
            strcat(skupaj, ":");
            sprintf(pomozen, "%d", (int)ntohs(cli_addr.sin_port));
            strcat(skupaj, pomozen);
            strcat(skupaj, "]");

            if(write(newsockfd, skupaj, 100) < 0){// pisemo nazaj odjemalcu
                error2("Napaka pri posiljanju client-u\n");
            }
            else{
                printf("Odgovoril sem: %s\n", skupaj);
            }

        }
        else if(str == 'B'){
            strcpy(skupaj, "Trenuten cas je: ");
            sprintf(pomozen, "%d", tm.tm_hour);
            strcat(skupaj, pomozen);
            strcat(skupaj, ":");
            sprintf(pomozen, "%d", tm.tm_min);
            strcat(skupaj, pomozen);
            strcat(skupaj, ":");
            sprintf(pomozen, "%d", tm.tm_sec);
            strcat(skupaj, pomozen);
            strcat(skupaj, " ");
            sprintf(pomozen, "%d", tm.tm_mday);
            strcat(skupaj, pomozen);
            strcat(skupaj, "-");
            sprintf(pomozen, "%d", tm.tm_mon+1);
            strcat(skupaj, pomozen);
            strcat(skupaj, "-");
            sprintf(pomozen, "%d", tm.tm_year+1900);
            strcat(skupaj, pomozen);

            if(write(newsockfd, skupaj, 100) < 0){// pisemo nazaj odjemalcu
                error2("Napaka pri posiljanju client-u\n");
            }
            else{
                printf("Odgovoril sem: %s\n", skupaj);
            }
        }
        else if(str == 'C'){
            if (getcwd(path, sizeof(path)) != NULL){

                strcpy(skupaj, "Direktorij: ");
                strcat(skupaj, path);

                if(write(newsockfd, skupaj, 100) < 0){// pisemo nazaj odjemalcu
                    error2("Napaka pri posiljanju client-u\n");
                }
                else{
                    printf("Odgovoril sem: %s\n", skupaj);
                }
            } 
            else{
                printf("Napaka pri iskanju delovnega direktorija\n");
            }
        }
        else if(str == 'D'){

            memmove(skupaj, buffer+1, strlen(buffer));
            
            if(write(newsockfd, skupaj, 100) < 0){// pisemo nazaj odjemalcu
                error2("Napaka pri posiljanju client-u\n");
            }
            else{
                printf("Odgovoril sem: %s\n", skupaj);
            }
        }
        else if(str == 'E'){

            if(uname(&uname_pointer) == 0){
                strcpy(skupaj, "Ime - ");
                strcat(skupaj, uname_pointer.nodename);
                strcat(skupaj, "\n");
                strcat(skupaj, "OS: - ");
                strcat(skupaj, uname_pointer.sysname);

                if(write(newsockfd, skupaj, 100) < 0){// pisemo nazaj odjemalcu
                    error2("Napaka pri posiljanju client-u\n");
                }
                else{
                    printf("Odgovoril sem: %s", skupaj);
                }
            }
            else{
                printf("Napaka pri iskanju sistemskih informacij\n");
            }
            
        }
        else if(str == 'F'){
                    printf("\n");
            //strcpy(skupaj, "\n");
            for(i=1; buffer[i] != ' '; i++){
                if(buffer[i] == '/'){
                    printf("\n");
                    //strcat(skupaj, "\n");
                }
                else
                {
                    if(buffer[i] > 48 && buffer[i] < 57)
                    {
                        pomoc = buffer[i] - '0';

                        for(j = 0; j<pomoc; j++){
                            //strcat(skupaj, " ");
                            printf(" ");
                        }
                    }
                    else if(buffer[i] > 64){
                        //strcat(skupaj, buffer[i]);
                        printf("%c", buffer[i]);
                    }
                }
            }

            if(buffer[i+1] == 'w'){
              printf("\nNa vrsti: beli\n");
            }
            else if(buffer[i+1] == 'b'){
              printf("\nNa vrsti: crni\n");
            }
            
            printf("\nMoznosti rokad:\n");

            for(i=i+3; buffer[i] != ' '; i++){
                if(buffer[i] == 'K'){
                    //strcat(skupaj, "beli, kraljeva stran\n");
                    printf("- beli, kraljeva stran\n");
                }
                else if(buffer[i] == 'Q'){
                    //strcat(skupaj, "beli, damina stran\n");
                    printf("- beli, damina stran\n");
                }
                else if(buffer[i] == 'k'){
                    //strcat(skupaj, "crni, kraljeva stran\n");
                    printf("- crni, kraljeva stran\n");
                }
                else if(buffer[i] == 'q'){
                    //strcat(skupaj, "crni, damina stran\n");
                    printf("- crni, damina stran\n");
                }
                else if(buffer[i] == '-'){
                    //strcat(skupaj, "\nNi moznosti rokad\n");
                    printf("- ni moznosti rokad\n");
                }
            }

            printf("Moznost en pasat: ");
            for(i=i+1; buffer[i] != ' '; i++){
                if(buffer[i] == '-'){
                    //strcat(skupaj, "Moznost en pasat: noben\n");
                    printf("noben");
                }
                else{
                    //strcat(skupaj, "Moznost en pasat: noben\n");
                    printf("%c", buffer[i]);
                }
            }

            for(i=i+1; buffer[i] != ' '; i++){
                printf("\nStevilo polpotez: %c\n", buffer[i]);
            }

            printf("Stevilo potez: ");
            for(i=i+1; buffer[i] != '\0'; i++){
                printf("%c", buffer[i]);
            }

            if(write(newsockfd, "Dobil sem tvoje sporocilo F sahovskih figur", 100) < 0){// pisemo nazaj odjemalcu
                error2("Napaka pri posiljanju client-u\n");
            }
            else{
                printf("Odgovoril sem: Dobil sem tvoje sporocilo F sahovskih figur");
            }  
            
        }
        else if(str == 'G'){
            if(write(newsockfd, "Dobil sem tvoje sporocilo G", 100) < 0){// pisemo nazaj odjemalcu
                error2("Napaka pri posiljanju client-u\n");
            }
            else{
                printf("Odgovoril sem: Dobil sem tvoje sporocilo G");
            }    
        }
        
        /*if(shutdown(newsockfd,0) == 0){// odklopimo odjemalca
            printf("\nOdjemalec se je odklopil.\n");
        }
        else{
            error("Napaka pri odklapljanju odjemalca\n");
        }*/
    }

    close(sockfd);// unicimo vticnico
    return 0; 
}