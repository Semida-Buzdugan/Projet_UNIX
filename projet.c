#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define MSGSIZE 256

/*for the exchanges by pipes*/
#define START_BONJOUR '0'
#define MISE_A_JOUR_PANIER '1'
#define TOTAL '2'
#define ACCUSE_RECEPTION '3'
#define BON_LIVRAISON '4'
#define LIVRAISON '5'
#define SIGNATURE_BONS '6'

/* DEFINITION OF THE SCENARIO */

/* Put or remove // : */

	/* Server : */
char *server = "sw1";
//char *server = "sw2";
//char *server = "sw3";

	/* Buyer : */
char *buyer = "Laure";
//char *buyer = "Philippe";
//char *buyer = "Paul";

	/* DeliveryDriver : */
char *deliveryDriver = "Xavier";
//char *deliveryDriver = "Michel";
//char *deliveryDriver = "France";

/* Put the quantities and prices for apple, orange and banana : */

typedef struct {
	char* name;
	float quantity;			/* the quantity, in kg, of the fruit to be bought */
	int stock;				/* the number of crates of the fruit */
	float price;			/* the price, in euros, of a kg of the fruit */
}Fruit;

	/* Quantities, stocks and prices of the articles : */

Fruit articles[3] = {{"apple", 1, 4, 7}, {"orange", 2, 5, 8}, {"banana", 3, 6, 9}};

/* END DEFINITION OF THE SCENARIO */


/* Variables of our program */

int p1[2], p2[2], p3[2], p4[2], p5[2], p6[2];

int pid_buyer, pid_server, pid_deliveryDriver;

int indexFruit = 0;

/* Functions of our program */

/* Checking that the scenario is correct */

void checkScenario(){
	if (articles[0].quantity <0 || articles[1].quantity <0 || articles[2].quantity <0){
		printf("Error : you entered a negative quantity\n");
		exit(1);
	}
	if (articles[0].quantity > articles[0].stock*10 || articles[1].quantity > articles[1].stock*10 || articles[2].quantity > articles[2].stock*20){
		printf("Error : you entered a quantity superior to the stocks\n");
		exit(1);
	}
	if (articles[0].price <0 || articles[1].price <0 || articles[2].price <0){
		printf("Error : you entered a negative price\n");
		exit(1);
	}
}

/* Creating a pipe */

void pipeSucceed(int p[2]){
	if (pipe(p) < 0){
		printf("Program. Pipe is not supported.\n");
		exit(1);
	}
}

/* Activating the non blocking mode */

void nonBlocking(int p[2]){
	if (fcntl(p[0], F_SETFL, O_NONBLOCK) < 0){
		printf("Program. Non blocking mode unavailable");
		exit(2);
	}
} 

/* Creating a child */

int forkSucceed(){
	int pid = fork();
	if (pid == -1){
		printf("Program. Fork impossible.\n");
		exit(3);
	}
	return pid;
}



/* The buyer enters the articles online */

void enterArticle(){
	printf("Buyer. Enter the article %s.\n", articles[indexFruit].name);
	write(p2[1], articles[indexFruit].name, sizeof(articles[indexFruit].name));
	printf("Message sent. Content = \"%s\".\n", articles[indexFruit].name);
	indexFruit++;
}

void serverAndBuyer_enter(){
	printf("Buyer. Closing reading access to tube.\n");
	close(p2[0]);
	
	printf("Buyer. Online entering.\n");
	for(int i = 0; i<3; i++){
		enterArticle();
		kill(pid_server, SIGUSR1);
	}
}

/* The server treats the articles entered */

void buyerAndServer_enter(){
	// Test :
	printf("Salut!\n");
}

/* Writing in a tube */

void writeInPipe(char message, int *pipe_name){
    char message_to_return[2];
    message_to_return[0] = message;
    message_to_return[1] = '\0';

    close(pipe_name[0]);
    write(pipe_name[1], message_to_return, sizeof(message_to_return));
}

/* Reading in a tube */

void readInPipe (int *pipe_name){
    char* message_read[MSGSIZE];
    read(pipe_name[0], message_read, MSGSIZE);
}

/*Function of Buyer processus*/
void Buyer(){
    /*TODO SEMIDA*/
    return;
}

/*Function of Server processus*/
void Server(){
    /*TODO SEMIDA*/
    return;
}

/* Function of DeliveryDriver processus*/
void DeliveryDriver(){
    /*TODO SEMIDA*/
    char buyer_message[2];

    close(p5[1]);
    read(p5[0], &buyer_message, sizeof(buyer_message));

    /* On evalue le premier caractere du message lu */
    /*switch(buyer_message[0]){
        case START_CONVERSATION:
            writeInPipe(HELLO, salesman_to_client);
            printf("Vendeur %s : Bonjour !\n");
            break;
        case HELLO:
            writeInPipe(ARTICLE, salesman_to_client);
            printf("Vendeur %s : Que puis-je faire pour vous ?\n");
            break;
        case ARTICLE:
            writeInPipe(GIVE, salesman_to_client);
            printf("Vendeur %s : Tenez voici un(e) %s, c'est l'un des meilleurs produits que nous proposons.\n", getArticleName(&article));
            break;
        default:
            break;
    }*/
    return;
}


int main (){
	checkScenario();

	printf("Beginning of the scenario.\n");
	
	printf("Program. Creating the pipes.\n");
	pipeSucceed(p1);	/* Reading : Buyer. Writing : Server */
	pipeSucceed(p2);	/* Reading : Server. Writing : Buyer */
	pipeSucceed(p3);	/* Reading : Server. Writing : Delivery driver */
	pipeSucceed(p4);	/* Reading : Delivery driver. Writing : Server */
	pipeSucceed(p5);	/* Reading : Buyer. Writing : Delivery driver */
	pipeSucceed(p6);	/* Reading : Delivery driver. Writing : Buyer */
	
	printf("Program. Activating the non blocking mode for each pipe.\n");
	nonBlocking(p1);
	nonBlocking(p2);
	nonBlocking(p3);
	nonBlocking(p4);
	nonBlocking(p5);
	nonBlocking(p6);
	
	printf("Program. Creating Delivery driver.\n");
	pid_deliveryDriver = forkSucceed();
	switch (pid_deliveryDriver){
		case 0 :
			sleep(2);
			printf("Delivery driver. Beginning.\n");
			/*TODO SEMIDA*/
			exit(0);
		default :
			printf("Program. Creating Server.\n");
			pid_server = forkSucceed();
			switch (pid_server){
				case 0 :
					sleep(1);
					printf("Server. Beginning.\n");
					signal(SIGUSR1, buyerAndServer_enter);
					exit(0);
				default :
					printf("Program. Creating Buyer.\n");
					pid_buyer = forkSucceed();
					switch (pid_buyer){
						case 0 :
							printf("Buyer. Beginning.\n");
							serverAndBuyer_enter();
							exit(0);
						default :
							sleep(1);
							exit(0);
					}
			}
			
	}

	return 0;
}