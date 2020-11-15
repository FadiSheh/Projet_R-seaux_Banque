#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 


//FONCTIONS MENU ET ACTIONS POSSIBLES
int menu(){

	int choix;
    printf("\n");
    printf("Gestion du compte : choix de l'action\n");
    printf("[1] Ajout d'une somme \n");
    printf("[2] Retrait d'une somme \n");
    printf("[3] Afficher solde\n");   
    printf("[4] Afficher 10 dernières opérations\n");
    printf("[5] Déconnexion\n");
    printf("\n");

    scanf("%d", &choix);
    return choix;
}

void ajoutSomme(int* nbCompte, float* somme){
		//on demande le compte puis la somme
	printf("Veuillez saisir le numéro de compte\n");
	scanf("%d", nbCompte);
	printf("Veuillez indiquer la somme à ajouter\n");
	scanf("%f", somme);
}

void retraitMontant(int* nbCompte, float* somme){
	printf("Veuillez saisir le numéro de compte\n");
	scanf("%d", nbCompte);
	printf("Veuillez indiquer la somme à retirer\n");
	scanf("%f", somme);
}

void recupNbCompte(int* nbCompte){
	printf("Veuillez saisir le numéro de compte\n");
	scanf("%d", nbCompte);
}



//MAIN
int main(int argc, char *argv[]){

	//declaration des variables
    int sockfd, portno, m, n;
   	int flag;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int nbCompte;
    float somme;
    char buffer[256];
    char receiveBuffer[256];

    //si nombre de variables invalide
	if (argc<4)
	{
		fprintf(stderr,"./client <server_address> <server_numport> <message>\n");
		fprintf(stderr,"ex: ./client localhost 32000 \"joe\"\n");
		return 1;
	}

	//recuperation du numero du port
    portno = atoi(argv[2]);

	//creation de la socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) 
	{
		fprintf(stderr,"Impossible d'ouvrir la socket. On arrête tout.\n");
		return 1;
	}
    
    //recuperation du numero du serveur
	server = gethostbyname(argv[1]);
    if (server == NULL) 
	{
        	fprintf(stderr,"Impossible de récupérer les infos sur le serveur\n");
		return 1;
    }

	// On donne toutes les infos sur le serveur
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

	//connexion de la socket
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
	{
		fprintf(stderr,"Impossible de faire l'appel system connect().\n");
		return 1;
	}


	//connexion : 0 est le premier element, id du client, mdp du client (1ere trame envoyée) (c'est quoi le 1?)
	//ETAPE DE CONNEXION
	char identifiant[10];
	char mdp[15];

	

	printf("Connexion au server\nBonjour %s\n", argv[3]);
	printf("Veuillez renseigner votre identifiant :\n");
    scanf("%s", identifiant);
	printf("Veuillez renseigner votre mdp:\n");
	scanf("%s", mdp);
	//printf("%s\n", mdp);
	
	//on stocke ces données dans un buffer
	sprintf(buffer, "%d %s %s", 0, identifiant, mdp);
    n = write(sockfd,buffer,strlen(buffer));

    printf("Données de connexion envoyées \n");
    printf("Attente de la réponse du server...\n");


	n = read(sockfd,receiveBuffer,256);

	close(sockfd);


	char verif_identifiants[15];


	printf("%s\n\n",receiveBuffer);

	sscanf(receiveBuffer,"%s",verif_identifiants);
	//demander l'action si la reponse du serveur est valide
	if(strcmp("OK",verif_identifiants) == 0){

		printf("Connexion Réussie\n");
		//si la premiere donnée est un 1 => on peut passer dans le menu
		m = menu();
		flag = 1;
	} else { printf("Identifiants Erronés\n");exit(0);}

	//en fonction de laction choisie on demande infos supp
	//FAIRE DES FONCTIONS
	while(flag){
		if(m == 1){
			//ajout d'une somme
			ajoutSomme(&nbCompte, &somme);
			//on change l'état du buffer
			//id_client  id_compte  password  somme
			sprintf(buffer, "%d %s %d %s %f", m, identifiant, nbCompte, mdp, somme);
			flag = 1;
		}
		else if(m == 2){
			//Retrait d'une somme
			//on demande le compte puis la somme
			retraitMontant(&nbCompte, &somme);
			//on change l'état du buffer
			//<id_client id_compte password somme>
			sprintf(buffer, "%d %s %d %s %f", m, identifiant, nbCompte, mdp, somme);
			flag = 1;			
		}
		else if (m == 3){
			//Afficher solde
			recupNbCompte(&nbCompte);
			//on change l'état du buffer
			//id_client id_compte password
			sprintf(buffer, "%d %s %d %s", m, identifiant, nbCompte, mdp);
			flag = 1;			
		}
		else if(m == 4){
			//Afficher 10 dernières opérations
			recupNbCompte(&nbCompte);
			//on change l'état du buffer
			//id_client id_compte password
			sprintf(buffer, "%d %s %d %s", m, identifiant, nbCompte, mdp);
			flag = 1;
		}
		else if (m == 5){
			//Déconnexion
			printf("Déconnexion en cours...\n");
			close(sockfd);
			flag = 0;
			return 1;
		}
		
		else if (m>5){
			printf("!! Veuillez choisir une action valide !!\n");
			
			flag = 1;	
		}

   

	//creation de la socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) 
	{
		fprintf(stderr,"Impossible d'ouvrir la socket. On arrête tout.\n");
		return 1;
	}

	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
	{
		fprintf(stderr,"Impossible de faire l'appel system connect().\n");
		return 1;
	}


    n = write(sockfd,buffer,strlen(buffer));
    printf("\nMessage envoyé\nAttente de la réponse\n");
    n = read(sockfd,receiveBuffer,256);
    
   	printf("\n%s", receiveBuffer);
	
	// On ferme la socket
    //printf("Déconnexion\n");
    close(sockfd);
    bzero(receiveBuffer,256);  

    printf("\n");   
    m = menu();




	}



    return 0;
}