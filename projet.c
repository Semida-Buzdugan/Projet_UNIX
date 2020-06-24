#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

/* DEFINITION DU SCENARIO */

/* Metre ou retirer des // : */

	/* Serveur : */
char *Server = "sw1";
//char *server = "sw2";
//char *server = "sw3";

	/* Acheteur : */
char *Buyer = "Laure";
//char *buyer = "Philippe";
//char *buyer = "Paul";

	/* Livreur : */
char *DeliveryDriver = "Xavier";
//char *deliveryDriver = "Michel";
//char *deliveryDriver = "France";

/* Mettre les quantités, stocks et prix pour chaque fruit: */

typedef struct {
	char *name;				/* le nom de l'article */
	float quantity;			/* la quantité, en kg de fruit acheté */
	int stock;				/* le nombre de cageot/caisse de fruit */
	float price;			/* le prix, en euros, d'un kg de fruit */
}Fruit;


Fruit articles[3] = {{"pomme", 1, 4, 7}, 
					 {"orange", 2, 5, 8}, 
					 {"banane", 3, 6, 9}};

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

/* Vérification du scénario: */

void checkScenario(){
	if (articles[0].quantity <0 || articles[1].quantity <0 || articles[2].quantity <0){
		printf("Erreur : vous avez entre une quantite negative.\n");
		exit(1);
	}
	if (articles[0].quantity > articles[0].stock*10 || articles[1].quantity > articles[1].stock*10 || articles[2].quantity > articles[2].stock*20){
		printf("Erreur : vous avez entre une quantite superieur au stock.\n");
		exit(1);
	}
	if (articles[0].price <0 || articles[1].price <0 || articles[2].price <0){
		printf("Erreur : vous avez entre un prix negatif.\n");
		exit(1);
	}
}

/* Création d'un tube: */

void pipeSucceed(int p[2]){
	if (pipe(p) < 0){
		printf("Programme. Le tube n'est pas supporte.\n");
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
				printf("Acheteur %s : Je saisis l'article %s.\n", Buyer, article.name);
			}
			break;
		case QUANTITY:
			writeInPipe(BASKET, p2);
			printf("Acheteur %s : Je désire %.2f kg de %s.\n", Buyer, article.quantity, article.name);
			iteration++;
			break;
		case PAYMENT:
			writeInPipe(DELIVERY_RECEIPT, p2);
			printf("Acheteur %s : Mon numero de carte bleu est **** et mon cryptogramme est ***.\n", Buyer);
			break;
		case DELIVERY_RECEIPT:
			printf("Acheteur %s : Accuse de reception de paiement recu!\n", Buyer);
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
			printf("Serveur %s : Il y a %d cageots/caisses de %s de disponible.\n", Server, article.stock, article.name);
			break;
		case BASKET :
			if (iteration <= 3){
				writeInPipe(ENTER_ARTICLE, p1);
			}
			printf("Serveur %s : Mise à jour du panier.\n", Server);
			printf("Serveur %s : Votre panier contient:\n", Server);
			receipt = 0;
			for (int i = 0; i<=iteration; i++){
				printf("		- %.2f kg de %s\n", articles[i].quantity, articles[i].name);
				receipt+=articles[i].quantity*articles[i].price;
			}
			iteration++;
			break;
		case DELIVERY_RECEIPT :
			writeInPipe(DELIVERY_RECEIPT, p1);
			printf("Serveur %s : Envoi de l'accuse de reception du paiement.\n", Server);
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
			printf("Livreur %s : Bons de livraison reçus.\n", DeliveryDriver);
			break;
	}
	
	writeInPipe(DELIVERY_AND_DELIVERY_NOTES, p5);
	printf("Livreur %s : Voici %s votre livraison ainsi que les bons associes.\n", DeliveryDriver, Buyer);
	
	return;
}

void buyerAndDeliveryDriver(){
	char deliveryDriver_message[2];
	
	close(p5[1]);
	read(p5[0], &deliveryDriver_message, sizeof(deliveryDriver_message));
	
	switch (deliveryDriver_message[0]){
		case DELIVERY_AND_DELIVERY_NOTES :
			printf("Acheteur %s : Livraison et bons recu.\n", Buyer);
			writeInPipe(SIGNATURE, p6);
			printf("Acheteur %s : Voici le bon signe %s.\n", Buyer, DeliveryDriver);
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
			printf("Livreur %s : Bon signe recu!\n", DeliveryDriver);
			break;
	}
	
	return;
}


int main (){
	checkScenario();
	
	/* Traitement du cas où l'acheteur ne veut pas acheter tous les types d'articles disponibles: */
	for(int i = 0; i<3; i++){
		// Semida
	}

	
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
					printf("Serveur %s : Fin de la saisie de vos articles. Votre facture totale est de %.2f euros.\n", Server, receipt);
					
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