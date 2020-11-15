#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>

// Données sur les clients
//time_t rawtime = time(NULL);


typedef struct{

    float montant;
    int type_operation;    //0 AJOUT 1 RETRAIT
    time_t date;

} case_tableau ;

struct _client {

    char identifiant[10];
    char mdp[15];

    case_tableau compte[2][11];

} clients[2];

int verif_identifiants(char* identifiant, char* mdp){


  for (int i = 0 ; i<2 ; i++){

    if ( strcmp(clients[i].identifiant,identifiant)==0){

        if (strcmp(clients[i].mdp,mdp) == 0){  return i;} 
            else {return -1;}
}}

return -1;}


void deplacer(int nbCompte,int indice_client){
  
    for (int i = 10 ;i>1;i--){

    clients[indice_client].compte[nbCompte][i]=clients[indice_client].compte[nbCompte][i-1];
   

    }
}


void ajout_montant(float somme,int nbCompte,int indice_client){


    deplacer(nbCompte,indice_client);

    clients[indice_client].compte[nbCompte][0].montant = clients[indice_client].compte[nbCompte][0].montant+somme;
    clients[indice_client].compte[nbCompte][0].type_operation = 0;
    clients[indice_client].compte[nbCompte][0].date = time(NULL);

    clients[indice_client].compte[nbCompte][1]=clients[indice_client].compte[nbCompte][0];
    clients[indice_client].compte[nbCompte][1].montant = somme;

    printf("Solde du client : %.f\n",clients[indice_client].compte[nbCompte][0].montant);
    printf("Type operation : %d\n",clients[indice_client].compte[nbCompte][0].type_operation);
   
    struct tm *ptm = localtime(&clients[indice_client].compte[nbCompte][0].date);

    printf("date: %02d/%02d/%02d\n",ptm->tm_mday,ptm->tm_mon,1900+ptm->tm_year);

}



void retrait_montant(float somme,int nbCompte,int indice_client){
deplacer(nbCompte,indice_client);

    clients[indice_client].compte[nbCompte][0].montant = clients[indice_client].compte[nbCompte][0].montant-somme;
    clients[indice_client].compte[nbCompte][0].type_operation = 1;
    clients[indice_client].compte[nbCompte][0].date = time(NULL);

    clients[indice_client].compte[nbCompte][1]=clients[indice_client].compte[nbCompte][0];
    clients[indice_client].compte[nbCompte][1].montant = somme;

     printf("Solde du client : %.f\n",clients[indice_client].compte[nbCompte][0].montant);
    printf("Type operation : %d\n",clients[indice_client].compte[nbCompte][0].type_operation);
   
    struct tm *ptm = localtime(&clients[indice_client].compte[nbCompte][0].date);

    printf("date: %02d/%02d/%02d\n",ptm->tm_mday,ptm->tm_mon,1900+ptm->tm_year);

}


case_tableau afficher_solde(int nbCompte,int indice_client){

return clients[indice_client].compte[nbCompte][0];

}



char * historique(int indice_client,int nbCompte,int indice){
char *buffer = malloc (sizeof (char) * 256);

char jma[11];

    case_tableau hist;

     hist = clients[indice_client].compte[nbCompte][indice];

     struct tm *ptm = localtime(&hist.date);
     sprintf(jma,"%02d/%02d/%02d",ptm->tm_mday,ptm->tm_mon,1900+ptm->tm_year);

            sprintf(buffer,"%s %d %s %.f\n","RES_SOLDE",hist.type_operation,jma, hist.montant);
            
            return buffer;

    

}
int main(int argc, char *argv[]){   

//Init client 
sprintf(clients[0].identifiant,"123");
//clients[0].identifiant = "123";
sprintf(clients[0].mdp,"321");
//clients[0].mdp = "321";
clients[0].compte[0][0].montant = 500;

for (int i = 1; i<11 ;i++){
    clients[0].compte[0][i].montant = 0;
    clients[0].compte[0][i].type_operation = 0;
    clients[0].compte[0][i].date = time(NULL);
}


    
    //printf("Identifiant: %s\nMot de passe: %s\nMontant: %f\n ",clients[0].identifiant,clients[0].mdp,clients[0].compte_1[0].montant);
    //Initialisation des différentes variables

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    char sendBuffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n,res_bind;
    case_tableau Val;

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


        //printf("Message reçu par %s:%d\nData: %s\n\n",inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buffer);


        //CONNEXION
        char identifiant[10];
        char mdp[15];
        int action;
        int ok=0;
        int nbCompte;
        float somme;
        int indice_client;
        float solde;
        char jma[11];

        sscanf(buffer,"%d",&action);
        bzero(sendBuffer,256); 

        printf("\n");
        printf("Message reçu part %s:%d\n\n",inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));


        if (action==0){

            printf("Le client se connecte\n");
            sscanf(buffer,"%d %s %s",&action,identifiant,mdp);
            printf("Action: %d   \nidentifiant: %s  \nMot de passe: %s \n",action,identifiant,mdp);
            indice_client= verif_identifiants(identifiant,mdp);
            printf("indice %d\n",indice_client);
            if (indice_client != -1){sprintf(sendBuffer,"OK\n");
            printf("Connexion acceptée\n");} else {sprintf(sendBuffer,"KO\n");printf("Connexion échouée\n");}
            n=write(newsockfd,sendBuffer,strlen(sendBuffer));


        }


        if (action==1){ 

            printf("Le client fait un ajout\n");
            sscanf(buffer,"%d %s %d %s %f",&action,identifiant,&nbCompte, mdp, &somme);
            printf("Action: %d\nIdentifiant: %s\nNcompte:%d\nMot de passe: %s\nSomme: %f \n",action,identifiant,nbCompte, mdp, somme); 
            if( (nbCompte>2) || (nbCompte<0)){ok = 0;} else { ok =1; ajout_montant(somme,nbCompte,indice_client);}
            if(ok ==1) {sprintf(sendBuffer,"OK\n");printf("Ajout Réussi\n");} else { sprintf(sendBuffer,"KO\n"); printf("Ajout Echoué\n");}
            n=write(newsockfd,sendBuffer,strlen(sendBuffer)); 
            

        }

        if (action==2){ 

            printf("Le client fait un retrait\n");
            sscanf(buffer,"%d %s %d %s %f",&action,identifiant,&nbCompte, mdp, &somme);
            printf("Action: %d\nIdentifiant: %s\nNcompte:%d\nMot de passe: %s\nSomme: %f \n",action,identifiant,nbCompte, mdp, somme);
            if( (nbCompte>2) || (nbCompte<0)){ok = 0;} else { ok =1;}
            retrait_montant(somme,nbCompte,indice_client);
            if(ok ==1) {sprintf(sendBuffer,"OK\n");printf("Retrait Réussi\n");} else { sprintf(sendBuffer,"KO\n"); printf("Retrait Echoué\n");}
            n=write(newsockfd,sendBuffer,strlen(sendBuffer));  
            

        }
      

        if (action==3){ 

            printf("Le client veut afficher son solde\n");
            sscanf(buffer,"%d %s %d %s",&action,identifiant,&nbCompte, mdp);
            printf("Action: %d\nIdentifiant: %s\nNcompte:%d\nMot de passe: %s\n",action,identifiant,nbCompte, mdp);
            if( (nbCompte>2) || (nbCompte<0)){ok = 0;} else { ok =1; Val = afficher_solde(nbCompte,indice_client);}

            struct tm *ptm = localtime(&Val.date);
            sprintf(jma,"%02d/%02d/%02d",ptm->tm_mday,ptm->tm_mon,1900+ptm->tm_year);

            sprintf(sendBuffer,"%s %.f %s","RES_SOLDE",Val.montant,jma);
            n=write(newsockfd,sendBuffer,strlen(sendBuffer)); 
        }

        if (action==4){ 

            printf("Le client veut afficher son historique\n");
            sscanf(buffer,"%d %s %d %s",&action,identifiant,&nbCompte, mdp);
            printf("Action: %d\nIdentifiant: %s\nNcompte:%d\nMot de passe: %s\n",action,identifiant,nbCompte, mdp);

            for(int i = 1; i<11;i++){
            sprintf(sendBuffer,"%s",historique(indice_client,nbCompte,i));
            n=write(newsockfd,sendBuffer,strlen(sendBuffer));  }
            
        }

   
        
        bzero(buffer,256);      
        close(newsockfd);
        
        }
        
        close(sockfd);
        return 0; 
}
