#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


int main(int argc, char *argv[])
{       
    //Initialisation des différentes variables

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    char sendBuffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n,res_bind;


    // Création de la socket TCP

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1){ printf("Erreur lors de l'ouverture de la socket\n"); return 1;}


    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi("32000");                                 //Le port par défaut pour le serveur est le port 32000
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;                 //Socket affectée à toutes les interfaces
    serv_addr.sin_port = htons(portno);



    res_bind = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if ( res_bind< 0){ printf("Erreur lors de l'appel système bind()\n"); return 1;}



    listen(sockfd,5);                                      //6 connexions pendantes maximum
    
    while (1){

        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (newsockfd < 0){ printf("Erreur lors de l'appel système accept()\n"); return 1;}


        bzero(buffer,256);                                //(Re)initialisation de buffer
        n = read(newsockfd,buffer,255);

        if (n < 0){ printf("Erreur lors de l'appel système read()\n"); return 1;}


        printf("Received packet from %s:%d\nData: [%s]\n\n",inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buffer);


        sprintf(sendBuffer,"Acquittement du message\n");
        n=write(newsockfd,sendBuffer,strlen(sendBuffer));   
        bzero(buffer,256);   
        bzero(sendBuffer,256);   
        close(newsockfd);
            
        }
        
        close(sockfd);
        return 0; 
}
