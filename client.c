#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

int main(int argc, char *argv[])
{
    	int sockfd, portno, m, n;
   		int flag;
    	struct sockaddr_in serv_addr;
    	struct hostent *server;
    	int nbCompte;
    	float somme;
    	char buffer[256];
    	char receiveBuffer[256];

	if (argc<4)
	{
		fprintf(stderr,"./client <server_address> <server_numport> <message>\n");
		fprintf(stderr,"ex: ./client localhost 32000 \"joe\"\n");
		return 1;
	}

    	portno = atoi(argv[2]);
	// 1) Création de la socket, INTERNET et TCP

    	sockfd = socket(AF_INET, SOCK_STREAM, 0);

    	if (sockfd < 0) 
	{
		fprintf(stderr,"Impossible d'ouvrir la socket. On arrête tout.\n");
		return 1;
	}
    
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

	// On se connecte. L'OS local nous trouve un numéro de port, grâce auquel le serveur
	// peut nous renvoyer des réponses

    	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
	{
		fprintf(stderr,"Impossible de faire l'appel system connect().\n");
		return 1;
	}

	//connexion : 1 est le premier element, id du client, mdp du client (1ere trame envoyée) (c'est quoi le 1?)
	//ETAPE DE CONNEXION
	char identifiant[10];
	char mdp[15];

	printf("Connexion au server\nBonjour %s\n", argv[3]);
	printf("Veuillez renseigner votre identifiant :\n");
	scanf("%s", identifiant);
	//printf("%s\n", identifiant);
	printf("Veuillez renseigner votre mdp:\n");
	scanf("%s", mdp);
	//printf("%s\n", mdp);
	printf("Attente de la réponse du server...\n");

	//on stocke ces données dans un buffer
	sprintf(buffer, "%d %s %s %s", 1, identifiant, mdp, argv[3]);
	//on envoie ces données au server qui les analysera
	n = write(sockfd,buffer,strlen(buffer));
	//reponse du server ok ou pas, analyser la trame reçue : si ok on passe dans le menu valeur 1 et 0 sinon
	n = read(sockfd, receiveBuffer, 256);
	printf("receiveBuffer='%s'",receiveBuffer);

	if(receiveBuffer[0]){
		//si la premiere donnée est un 1 => on peut passer dans le men
		m = menu();
		flag = 1;
	}
	
	//demander l'action
	//en fonction de laction choisie on demande infos supp
	//FAIRE DES FONCTIONS
	while(flag){
		if(m == 1){
			//ajout d'une somme
			//on demande le compte puis la somme
			printf("Veuillez saisir le numéro de compte\n");
			scanf("%d", &nbCompte);
			printf("Veuillez indiquer la somme à ajouter\n");
			scanf("%f", &somme);
			//on change l'état du buffer
			//id_client  id_compte  password  somme
			sprintf(buffer, "%s %d %s %f", identifiant, nbCompte, mdp, somme);
			flag = 0;
		}
		else if(m == 2){
			//Retrait d'une somme
			//on demande le compte puis la somme
			printf("Veuillez saisir le numéro de compte\n");
			scanf("%d", &nbCompte);
			printf("Veuillez indiquer la somme à retirer\n");
			scanf("%f", &somme);
			//on change l'état du buffer
			//<id_client id_compte password somme>
			sprintf(buffer, "%s %d %s %f", identifiant, nbCompte, mdp, somme);
			flag = 0;
		}
		else if (m == 3){
			//Afficher solde
			printf("Veuillez saisir le numéro de compte\n");
			scanf("%d", &nbCompte);
			//on change l'état du buffer
			//id_client id_compte password
			sprintf(buffer, "%s %d %s", identifiant, nbCompte, mdp);
			flag = 0;
		}
		else if(m == 4){
			//Afficher 10 dernières opérations
			printf("Veuillez saisir le numéro de compte\n");
			scanf("%d", &nbCompte);
			//on change l'état du buffer
			//id_client id_compte password
			sprintf(buffer, "%s %d %s", identifiant, nbCompte, mdp);
			flag = 0;
		}
		else if (m == 5){
			//Déconnexion
			printf("Déconnexion en cours...\n");
			close(sockfd);
		}
		else{
			printf("!! Veuillez choisir une action valide !!\n");
			m = menu();
			flag = 1;
		}
	}
	/*printf("%d\n", m);
	printf("%s\n", argv[0]);
	printf("%s\n", argv[1]);
	printf("%s\n", argv[2]);
	printf("%s\n", argv[3]);*/



	//affichage des données concernant le client ainsi que son action désirée
	//sprintf(buffer, "%d %d %d %s %d", );
	//sprintf(buffer, "%d %d %d %s %d",1, 3, 67, "coucou", 100);
	//sprintf(buffer,"%d\n", m);

	printf("buffer : %s", buffer);

	
	//sprintf(buffer,"%d %d %s %d\n", argv[1], argv[2], argv[3], m);

    n = write(sockfd,buffer,strlen(buffer));

    n = read(sockfd,receiveBuffer,256);

    printf("Résultat de votre requête :\n");
	printf("receiveBuffer='%s'",receiveBuffer);

	// On ferme la socket

    	close(sockfd);

    	return 0;
}

int menu(){

	int choix;

    printf("Gestion du compte : choix de l'action\n");
    printf("Ajout d'une somme (1)\n");
    printf("Retrait d'une somme (2)\n");
    printf("Afficher solde (3)\n");   
    printf("Afficher 10 dernières opérations (4)\n");
    printf("Déconnexion (5)\n");

    scanf("%d", &choix);
    return choix;
}