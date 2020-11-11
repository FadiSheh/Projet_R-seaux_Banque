#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

// Données sur les clients

typedef struct {

    int numero;
    int montant;
    float historique[10];

} compte;

struct _client {

    char identifiant[10];
    char mdp[15];

    compte* compte_1;
    compte* compte_2;

} clients[2];



/*clients[1].identifiant = "789";
clients[1].mdp = "789";
clients[1].compte_1 = 500;
clients[1].compte_2 = 530;
*/



int main(int argc, char *argv[]){     

/*sprintf(clients[0].identifiant,"123\0");
//clients[0].identifiant = "123\0";
sprintf(clients[0].mdp,"321\0");
//clients[0].mdp = "321\0";
clients[0].compte_1->numero = 1;
clients[0].compte_2->numero = 2;
clients[0].compte_1->montant = 350;
clients[0].compte_2->montant = 938;  

    printf("Identifiant: %s \nMot De passe: %s \nNum compte 1: %d\nMontant 1:%d\nNum compte 2: %d\nMontant 2:%d\n",clients[0].identifiant,clients[0].mdp,clients[0].compte_1->numero,clients[0].compte_1->montant,clients[0].compte_2->numero,clients[0].compte_2->montant);
*/

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


        printf("Message reçu par %s:%d\nData: %s\n\n",inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buffer);


        //CONNEXION
        char identifiant[10];
        char mdp[15];
        int action;


        sscanf(buffer,"%d %s %s",&action,identifiant,mdp);
        printf("Action: %d   \nidentifiant: %s  \n  Mot de passe: %s \n",action,identifiant,mdp);

        // VERIFIER QUE LES IDENTIFIANTS SONT BONS

        //si c'est bon 
        sprintf(sendBuffer,"Identifiants Corrects\n");
        n=write(newsockfd,sendBuffer,strlen(sendBuffer));  

        


        bzero(buffer,256);   
        bzero(sendBuffer,256);   
        close(newsockfd);
            
        }
        
        close(sockfd);
        return 0; 
}
