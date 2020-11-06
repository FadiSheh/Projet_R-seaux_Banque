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
     	int sockfd, newsockfd, portno;
     	socklen_t clilen;
     	char buffer[256];
     	char sendBuffer[256];
     	struct sockaddr_in serv_addr, cli_addr;
     	int n;

        
        printf("Demarrage du Serveur sur le port 32000 (défaut)");
        
	// Creation de la socket SOCK_STREAM 
     	sockfd = socket(AF_INET, SOCK_STREAM, 0); //SOCKSTREAM correspond au tcp

     	while(sockfd < 0){

         	printf("Erreur Ouverture Socket.\n");
            sockfd = socket(AF_INET, SOCK_STREAM, 0);

	}


        printf("Création Socket Reussie\n");



	// 2) on réclame au noyau l'utilisation du port passé en paramètre 
	// INADDR_ANY dit que la socket va être affectée à toutes les interfaces locales

     	bzero((char *) &serv_addr, sizeof(serv_addr));
     	portno = atoi(argv[1]);
     	serv_addr.sin_family = AF_INET;
     	serv_addr.sin_addr.s_addr = INADDR_ANY;
     	serv_addr.sin_port = htons(portno);

     	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
	{
         	printf("Impossible de faire l'appel système bind().\n");
		return 1;
	}

       

	// On commence à écouter sur la socket. Le 5 est le nombre max
	// de connexions pendantes

     	listen(sockfd,5);

     	while (1)
     	{    
     		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     		if (newsockfd < 0) 
		{
         		fprintf(stderr,"Impossible de faire l'appel système accept().\n");
			return 1;
		}

          
     		bzero(buffer,256); //vide le buffer
     		n = read(newsockfd,buffer,255);

     		if (n < 0) 
		{
         		fprintf(stderr,"Impossible de faire l'appel système read().\n");
			return 1;
		}

        	printf("Received packet from %s:%d\nData: %s\n\n",
                	inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buffer);



            int id_client;
            int id_compte;
            int somme;
            char password[50];
            int action;

            sscanf(buffer, "%d %d %d %s %d",&action, &id_client,&id_compte,password,&somme);

            printf("Action : %d \nId Client : %d \n Id Compte : %d \n Mot de Passe saisi : %s\n Montant à gérer %d",action, id_client, id_compte, password, somme );



            //Verification id et mot de passe

            sscanf(buffer, "%d", &action);

            




		sprintf(sendBuffer,"Acquittement du message\n");
		//n=write(newsockfd,sendBuffer,strlen(sendBuffer));	

     		close(newsockfd);
     	}

     	close(sockfd);
     	return 0; 
}




