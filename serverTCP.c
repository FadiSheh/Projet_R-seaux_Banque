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



//Structure  case_tableau qui correspond à une transaction

typedef struct{           

    float montant;
    int type_operation;    //0 AJOUT 1 RETRAIT
    time_t date;

} case_tableau ;


//Structure client qui represente un client
struct _client {

    char identifiant[10];      
    char mdp[15];
    case_tableau compte[2][11];    //2 Comptes, un historique de 10 transactins. La première case de chaque ligne contient le montant présent sur le compte

} clients[2];



//Vérifie que l'identifiant existe et que le mdp associé correspond.
int verif_identifiants(char* identifiant, char* mdp){

  for (int i = 0 ; i<2 ; i++){

    if ( strcmp(clients[i].identifiant,identifiant)==0){

        if (strcmp(clients[i].mdp,mdp) == 0){  return i;} // S'il retrouve l'idenfiant et que le mdp est bon on renvoit son indice
            else {

            	return -1;   //si le mdp est erroné on renvoit -1
            }
		}
    }

return -1; //si l'identifiant n'existe pas on renvoit -1
}



//Déplace chaque case de tableau vers la droite pour mettre à jour l'historique des transactions
void deplacer(int nbCompte,int indice_client){
  
    for (int i = 10 ;i>1;i--){

    clients[indice_client].compte[nbCompte][i]=clients[indice_client].compte[nbCompte][i-1];
   
    }
}



//On ajoute le montant
void ajout_montant(float somme,int nbCompte,int indice_client){


    deplacer(nbCompte,indice_client); //on deplace toutes les cases vers la droite 


    //On enregistre les données de la transactions dans la premiere case du tableau
    clients[indice_client].compte[nbCompte][0].montant = clients[indice_client].compte[nbCompte][0].montant+somme;
    clients[indice_client].compte[nbCompte][0].type_operation = 0;
    clients[indice_client].compte[nbCompte][0].date = time(NULL);


    // Puis on la deplace dans la case à coté pour mettre a jour l'historique
    clients[indice_client].compte[nbCompte][1]=clients[indice_client].compte[nbCompte][0];
    clients[indice_client].compte[nbCompte][1].montant = somme;


    // Affichage des informations dans le terminal
    printf("Solde du client : %.f\n",clients[indice_client].compte[nbCompte][0].montant);
    printf("Type operation : %d\n",clients[indice_client].compte[nbCompte][0].type_operation);
   
    struct tm *ptm = localtime(&clients[indice_client].compte[nbCompte][0].date);

    printf("date: %02d/%02d/%02d\n",ptm->tm_mday,ptm->tm_mon,1900+ptm->tm_year);

}




int retrait_montant(float somme,int nbCompte,int indice_client){

	//Meme procedure que l'ajout mais avec le retrait de la somme
	if( clients[indice_client].compte[nbCompte][0].montant - somme >= 0){

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
    return 1;
    } else {return -1;}

}


case_tableau afficher_solde(int nbCompte,int indice_client){

	//Fonction qui permet de recuperer le solde du client en question
	return clients[indice_client].compte[nbCompte][0];

}



char * historique(int indice_client,int nbCompte,int indice){

	char *buffer = malloc (sizeof (char) * 512);
	bzero(buffer,512);
	char jma[11];
	char fonction[10];
 	case_tableau hist;

    hist = clients[indice_client].compte[nbCompte][indice]; //recuperation des données

     struct tm *ptm = localtime(&hist.date);
     sprintf(jma,"%02d/%02d/%02d",ptm->tm_mday,ptm->tm_mon,1900+ptm->tm_year); //Affichage sur le terminal

     if (hist.type_operation == 0){ sprintf(fonction,"%s","AJOUT"); } else {sprintf(fonction,"%s","RETRAIT");} //On place dans un buffer le type de l'operation

            sprintf(buffer,"%s %s %s %.f\n","RES_OPERATIONS",fonction,jma, hist.montant);
            
            return buffer;   

}


int main(int argc, char *argv[]){   

	

	for (int i = 0; i<11 ;i++){
		//Initialisation des comptes des différents clients

    	clients[0].compte[0][i].montant = 0;
    	clients[0].compte[0][i].type_operation = 0;
    	clients[0].compte[0][i].date = time(NULL);

    	clients[0].compte[1][i].montant = 0;
    	clients[0].compte[1][i].type_operation = 0;
    	clients[0].compte[1][i].date = time(NULL);


    	clients[1].compte[0][i].montant = 0;
    	clients[1].compte[0][i].type_operation = 0;
    	clients[1].compte[0][i].date = time(NULL);

    	clients[1].compte[1][i].montant = 0;
    	clients[1].compte[1][i].type_operation = 0;
    	clients[1].compte[1][i].date = time(NULL);
	}

//Init clients 
	sprintf(clients[0].identifiant,"Fadi");
	sprintf(clients[0].mdp,"mdpfadi");
	clients[0].compte[0][0].montant = 500;

	sprintf(clients[1].identifiant,"Amelie");
	sprintf(clients[1].mdp,"mdpamelie");
	clients[1].compte[0][0].montant = 500;

    //Initialisation des différentes variables

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[512];
    char sendBuffer[512];
    struct sockaddr_in serv_addr, cli_addr;
    int n,res_bind;
    case_tableau Val;
    char identifiant[10];
    char mdp[15];
    int action;
    int ok=0;
    int nbCompte;
    float somme;
    int indice_client;
    float solde;
    char jma[11];

    // Création de la socket TCP

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1){ printf("Erreur lors de l'ouverture de la socket\n"); return 1;}


    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi("32000");                                 //Le port par défaut pour le serveur est le port 32000
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;                 //Socket affectée à toutes les interfaces
    serv_addr.sin_port = htons(portno);



    res_bind = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));      //BIND
    if ( res_bind< 0){ printf("Erreur lors de l'appel système bind()\n"); return 1;}


    listen(sockfd,5);                                      //5 connexions pendantes maximum
 
    printf("\nServeur Démarré\n");



    while (1){

        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0){ printf("Erreur lors de l'appel système accept()\n"); return 1;}


        bzero(buffer,512);                                //(Re)initialisation de buffer
        n = read(newsockfd,buffer,255);

        if (n < 0){ printf("Erreur lors de l'appel système read()\n"); return 1;}

        

        sscanf(buffer,"%d",&action); //Récupération de l'action qui est ici une connexion
        bzero(sendBuffer,512); 

        printf("\n");
        printf("Message reçu part %s:%d\n\n",inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));



        if (action==0){

        	//Verification de la connexion
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

        	//AJOUT
            printf("Le client fait un ajout\n");
            sscanf(buffer,"%d %s %d %s %f",&action,identifiant,&nbCompte, mdp, &somme);
            indice_client= verif_identifiants(identifiant,mdp);
            printf("Action: %d\nIdentifiant: %s\nNcompte:%d\nMot de passe: %s\nSomme: %f \n",action,identifiant,nbCompte, mdp, somme); 
            if( (nbCompte>2) || (nbCompte<0)){ok = 0;} else { ok =1; ajout_montant(somme,nbCompte,indice_client);}
            if(ok ==1) {sprintf(sendBuffer,"OK\n");printf("Ajout Réussi\n");} else { sprintf(sendBuffer,"KO\n"); printf("Ajout Echoué\n");}
            n=write(newsockfd,sendBuffer,strlen(sendBuffer)); 
            

        }

        if (action==2){ 

        	//RETRAIT
            printf("Le client fait un retrait\n");
            sscanf(buffer,"%d %s %d %s %f",&action,identifiant,&nbCompte, mdp, &somme);
            indice_client= verif_identifiants(identifiant,mdp);
            printf("Action: %d\nIdentifiant: %s\nNcompte:%d\nMot de passe: %s\nSomme: %f \n",action,identifiant,nbCompte, mdp, somme);
            if( (nbCompte>2) || (nbCompte<0)){ok = 0;} else { ok =1;}
            int okbis = retrait_montant(somme,nbCompte,indice_client);
            if((ok ==1) && (okbis==1)) {sprintf(sendBuffer,"OK\n");printf("Retrait Réussi\n");} else { sprintf(sendBuffer,"KO\n"); printf("Retrait Echoué\n");}
            n=write(newsockfd,sendBuffer,strlen(sendBuffer));  
            

        }
      

        if (action==3){ 

        	//AFFICHAGE SOLDE
            printf("Le client veut afficher son solde\n");
            sscanf(buffer,"%d %s %d %s",&action,identifiant,&nbCompte, mdp);
            indice_client= verif_identifiants(identifiant,mdp);
            printf("Action: %d\nIdentifiant: %s\nNcompte:%d\nMot de passe: %s\n",action,identifiant,nbCompte, mdp);
            if( (nbCompte>2) || (nbCompte<0)){ok = 0;} else { ok =1; Val = afficher_solde(nbCompte,indice_client);}

            struct tm *ptm = localtime(&Val.date);
            sprintf(jma,"%02d/%02d/%02d",ptm->tm_mday,ptm->tm_mon,1900+ptm->tm_year);

            sprintf(sendBuffer,"%s %.f %s","RES_SOLDE",Val.montant,jma);
            n=write(newsockfd,sendBuffer,strlen(sendBuffer)); 
        }

        if (action==4){ 

        	//AFFICAHGE HISTORIQUE
            printf("Le client veut afficher son historique\n");
            sscanf(buffer,"%d %s %d %s",&action,identifiant,&nbCompte, mdp);
            indice_client= verif_identifiants(identifiant,mdp);
            printf("Action: %d\nIdentifiant: %s\nNcompte:%d\nMot de passe: %s\n",action,identifiant,nbCompte, mdp);

            for(int i = 1; i<11;i++){

                printf("%s \n",historique(indice_client,nbCompte,i));

            }

            for(int i = 1; i<11;i++){
            bzero(sendBuffer,512); 
            sprintf(sendBuffer,"%s",historique(indice_client,nbCompte,i));
            n=write(newsockfd,sendBuffer,strlen(sendBuffer));  }
            
        }

   
        
        bzero(buffer,512);      
        close(newsockfd);
        
        }
        
        close(sockfd);
        return 0; 
}
