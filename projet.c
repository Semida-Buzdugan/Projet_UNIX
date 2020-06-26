#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

/* DEFINITION DU SCENARIO */

/* Metre ou retirer des // : */

	/* Serveur : */
char *Servers[3] = {"sw1", "sw2", "sw3"}; 
char *Server = NULL;

	/* Acheteur : */
char *Buyers[3] = {"Laure", "Philippe", "Paul"}; 
char *Buyer = NULL;

	/* Livreur : */
char *DeliveryDrivers[3] = {"Xavier", "Michel", "France"}; 
char *DeliveryDriver = NULL;

/* Mettre les quantités, stocks et prix pour chaque fruit: */

typedef struct {
	char *name;				/* le nom de l'article */
	int quantity;			/* la quantité, en cageot(s)/caisse(s) de fruit acheté */
	int stock;				/* le nombre de cageot/caisse de fruit */
	float price;			/* le prix, en euros, d'un(e) cageot(s)/caisse(s) de fruit */
}Fruit;


Fruit articles[3] = {{"pomme", 0, 0, 0}, 
					 {"orange", 0, 0, 0}, 
					 {"banane", 0, 0, 0}};

/* FIN DEFINITION DU SCENARIO */

/* VARIABLES DU PROGRAMME: */

#define ENTER_ARTICLE '0'
#define STOCK '1'
#define QUANTITY '2'
#define BASKET '3'
#define PAYMENT '4'
#define DELIVERY_RECEIPT '5'
#define DELIVERY_NOTES '6'
#define DELIVERY_AND_DELIVERY_NOTES '7'
#define SIGNATURE '8'

Fruit article;

int p1[2], p2[2], p3[2], p4[2], p5[2], p6[2];

int pid_buyer, pid_deliveryDriver;

int iteration = 0;

float receipt = 0;

/* FONCTIONS DU PROGRAMME: */

/* Choix du scénario : quel acheteur, quel vendeur, quel livreur */

char* chose (char** tab){
    srand(time(NULL));
    int i = rand()%3;
    return tab[i];
}

/* Choix du stock */

int choseRand(int max){
    srand(time(NULL));
    int i;
    do{
        i = rand()%max;
    }while(i==0);
    return i;
}

/* Création d'un tube: */

void pipeSucceed(int p[2]){
	if (pipe(p) < 0){
		printf("Programme. Le tube n'est pas support\u00e9.\n");
		exit(1);
	}
}

/* Acctivation du mode non bloquant: */

void nonBlocking(int p[2]){
	if (fcntl(p[0], F_SETFL, O_NONBLOCK) < 0){
		printf("Programme. Mode non bloquant indisponible.\n");
		exit(2);
	}
} 

/* Création d'un enfant: */

int forkSucceed(){
	int pid = fork();
	if (pid == -1){
		printf("Programme. Fork impossible.\n");
		exit(3);
	}
	return pid;
}

/* Ecriture dans un pipe: */

void writeInPipe(char message, int *p){
	char message_to_write[2];
	message_to_write[0] = message;
	message_to_write[1] = '\0';
	
	close(p[0]);
	write(p[1], message_to_write, sizeof(message_to_write));
}

/* Mise à jour d'article : */
void update(){
	article = articles[iteration];
}

/* Actions de l'acheteur: */

void buyer(){
	update();
	
	char server_message[2];
	
	close(p1[1]);
	read(p1[0], &server_message, sizeof(server_message));
	
	switch(server_message[0]){
		case ENTER_ARTICLE:
			if (iteration <= 2){
				writeInPipe(STOCK, p2);
				printf("\nAcheteur %s : Je saisis l'article %s.\n", Buyer, article.name);
			}
			break;
		case QUANTITY:
			writeInPipe(BASKET, p2);
			printf("Acheteur %s : Je d\u00e9sire %d cageot(s)/caisse(s) de %ss.\n", Buyer, article.quantity, article.name);
			iteration++;
			break;
		case PAYMENT:
			writeInPipe(DELIVERY_RECEIPT, p2);
			printf("Acheteur %s : Mon num\u00e9ro de carte bleue est **** et mon cryptogramme est ***.\n", Buyer);
			break;
		case DELIVERY_RECEIPT:
			printf("Acheteur %s : Accus\u00e9 de r\u00e9ception de paiement re\u00e7u!\n\n", Buyer);
	}
	
	return;
}

/* Actions serveur: */

void serverAndBuyer(){
	update();
	
	char buyer_message[2];
	
	close(p2[1]);
	read(p2[0], &buyer_message, sizeof(buyer_message));
	
	switch(buyer_message[0]){
		case STOCK :
			writeInPipe(QUANTITY, p1);
			printf("Serveur %s : Il y a %d cageot(s)/caisse(s) de %s disponibles.\n", Server, article.stock, article.name);
			break;
		case BASKET :
			if (iteration <= 3){
				writeInPipe(ENTER_ARTICLE, p1);
			}
			printf("Serveur %s : Mise \u00e0 jour du panier.\n", Server);
			printf("Serveur %s : Votre panier contient:\n", Server);
			receipt = 0;
			for (int i = 0; i<=iteration; i++){
				printf("		- %d cageot(s)/caisse(s) de %ss\n", articles[i].quantity, articles[i].name);
				receipt+=articles[i].quantity*articles[i].price;
			}
			iteration++;
			break;
		case DELIVERY_RECEIPT :
			writeInPipe(DELIVERY_RECEIPT, p1);
			printf("Serveur %s : Envoi de l'accus\u00e9 de r\u00e9ception du paiement.\n", Server);
			printf("		ACCUSE DE RECEPTION :	Montant : %.2f euros\n", receipt);
			break;
	}
	
	return;
}

void deliveryDriverAndServer(){
	char server_message[2];
	
	close(p4[1]);
	read(p4[0], &server_message, sizeof(server_message));
	
	switch(server_message[0]){
		case DELIVERY_NOTES :
			printf("Livreur %s : Bons de livraison et liste d'articles re\u00e7us en double exemplaire.\n", DeliveryDriver);
			printf("Les articles commandés sont : %d cageot(s) de %ss, %d cageot(s) d'%ss et %d caisse(s) de %ss \n\n", articles[0].quantity, articles[0].name, articles[1].quantity, articles[1].name, articles[2].quantity, articles[2].name);
			break;
	}
	
	writeInPipe(DELIVERY_AND_DELIVERY_NOTES, p5);
	printf("Livreur %s : Voici %s votre livraison ainsi que les bons associ\u00e9s.\n", DeliveryDriver, Buyer);
	
	return;
}

void buyerAndDeliveryDriver(){
	char deliveryDriver_message[2];
	
	close(p5[1]);
	read(p5[0], &deliveryDriver_message, sizeof(deliveryDriver_message));
	
	switch (deliveryDriver_message[0]){
		case DELIVERY_AND_DELIVERY_NOTES :
			printf("Acheteur %s : Livraison et bons re\u00e7u.\n", Buyer);
			writeInPipe(SIGNATURE, p6);
			printf("Acheteur %s : Voici le bon sign\u00e9 %s.\n", Buyer, DeliveryDriver);
			break;
	}
		
	return;
}

void deliveryDriverAndBuyer(){
	char buyer_message[2];
	
	close(p6[1]);
	read(p6[0], &buyer_message, sizeof(buyer_message));
	
	switch(buyer_message[0]){
		case SIGNATURE :
			printf("Livreur %s : Bon sign\u00e9 re\u00e7u !\n", DeliveryDriver);
			break;
	}
	
	return;
}


int main (){
	
	/* Traitement du cas où l'acheteur ne veut pas acheter tous les types d'articles disponibles: */
	for(int i = 0; i<3; i++){
		// Semida
	}

	//Création des pipes en mode non bloquant
	
	pipeSucceed(p1);	/* Lecture : Acheteur. Ecriture : Serveur */
	pipeSucceed(p2);	/* Lecture : Serveur. Ecriture : Acheter */
	pipeSucceed(p3);	/* Lecture : Serveur. Ecriture : Livreur */
	pipeSucceed(p4);	/* Lecture : Livreur. Ecriture : Serveur */
	pipeSucceed(p5);	/* Lecture : Acheteur. Ecriture : Livreur */
	pipeSucceed(p6);	/* Lecture : Livreur. Ecriture : Acheteur */
	
	nonBlocking(p1);
	nonBlocking(p2);
	nonBlocking(p3);
	nonBlocking(p4);
	nonBlocking(p5);
	nonBlocking(p6);
	
	//CHOIX SCENARIO 
	
	printf("CHOIX DU SCENARIO \n\n");
	Server = chose(Servers);
	sleep(1); //On utilise cette commande pour avoir de bonnes valeurs aléatoires
	Buyer = chose(Buyers);
	sleep(1);
	DeliveryDriver = chose(DeliveryDrivers);
	printf("Le sc\u00e9nario choisi est le suivant : %s ach\u00e8te des fruits sur le serveur %s et est livr\u00e9 par %s. \n", Buyer, Server, DeliveryDriver);
	
	for (int i=0; i<3; i++){
	    articles[i].stock = choseRand(50); //On choisi arbitrairement que le stock maximum qu'on puisse avoir est 50.
	    articles[i].quantity = choseRand(articles[i].stock);
	    articles[i].price = rand()%(10-5)+5; //Le prix est choisi aléatoirement entre 5 et 10 euros.
	    sleep(1);
	}
	printf("En stock il y a %d cageot(s) de %ss, %d cageot(s) d'%ss et %d caisse(s) de %ss \n", articles[0].stock, articles[0].name, articles[1].stock, articles[1].name, articles[2].stock, articles[2].name);
	
	pid_buyer = forkSucceed();
	switch(pid_buyer){
		case 0 :
			/* ------- ACHETEUR -------  */
		
			/* Interaction avec le serveur : */
			for(int i = 0; i< 5; i++){
				signal(SIGUSR1, buyer);
				pause();
			}
			for(int i = 0; i< 4; i++){
				signal(SIGUSR1, buyer);
				pause();
			}
			
			/* Interaction avec le livreur : */
			signal(SIGUSR1, buyerAndDeliveryDriver);
			pause();
			
			exit(0);
		
		default :
			sleep(1);
			pid_deliveryDriver = forkSucceed();
			switch(pid_deliveryDriver){
				case 0:
					/* ------- LIVREUR ------- */
					
					signal(SIGUSR1, deliveryDriverAndServer);
					pause();
					
					signal(SIGUSR1, deliveryDriverAndBuyer);
					pause();
					
					exit(0);
					
				default :
					/* ------- SERVEUR -------  */
					
					sleep(1);
					
					/* Interaction avec l'acheteur: */
					writeInPipe(ENTER_ARTICLE, p1);
					for (int i=0; i<6; i++){
						kill(pid_buyer, SIGUSR1);
						sleep(1);
						
						serverAndBuyer();
					}
					printf("\nServeur %s : Fin de la saisie de vos articles. Voici votre facture\n", Server);
					printf("Serveur %s : - %ss - %d cageot(s)/caisse(s) - %.2f PU - %.2f € \n", Server, articles[0].name, articles[0].quantity, articles[0].price, articles[0].quantity*articles[0].price);
					printf("            : - %ss - %d cageot(s)/caisse(s) - %.2f PU - %.2f € \n", articles[1].name, articles[1].quantity, articles[1].price, articles[1].quantity*articles[1].price);
					printf("            : - %ss - %d cageot(s)/caisse(s) - %.2f PU - %.2f € \n", articles[2].name, articles[2].quantity, articles[2].price, articles[2].quantity*articles[2].price);
					printf("Le total est de %.2f euros.\n", receipt);
					
					/* 7) */
					writeInPipe(PAYMENT, p1);
					kill(pid_buyer, SIGUSR1);
					sleep(1);
					
					/* 8) */
					serverAndBuyer();
					kill(pid_buyer, SIGUSR1);
					sleep(1);
					
					serverAndBuyer();
					kill(pid_buyer, SIGUSR1);
					sleep(1);
					
					/* Interaction avec le livreur: */
					/* 9) */
					writeInPipe(DELIVERY_NOTES, p4);
					printf("Server %s : Transmission des bons de livraisons.\n", Server);
					kill(pid_deliveryDriver, SIGUSR1);
					sleep(1);
					
					/* Transmission des signaux entre livreur et acheteur : */
					/* 10) */
					kill(pid_buyer, SIGUSR1);
					sleep(1);					
                    
                    /* 11) */
					kill(pid_deliveryDriver, SIGUSR1);
					sleep(1);
					
					exit(0);
			}
	}
	
	return 0;
}