#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/* DEFINING SCENARIO */

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
	float quantity;			/* the quantity, in kg, of the fruit to be bought */
	int stock;				/* the number of crates of the fruit */
	float price;			/* the price, in euros, of a kg of the fruit */
}Fruit;

	/* Quantities, stocks and prices of the articles : */

Fruit apple = {1, 4, 7};	/* Write first the number of kg of fruits bought, then the number of crates and finally the price of a kg */

Fruit orange = {2, 5, 8};

Fruit banana = {3, 6, 9};


/* Checking that the scenario is correct */

void checkScenario(){
	if (apple.quantity <0 || orange.quantity <0 || banana.quantity <0){
		printf("Error : you entered a negative quantity\n");
		exit(1);
	}
	if (apple.quantity > apple.stock*10 || orange.quantity > orange.stock*10 || banana.quantity > banana.stock*20){
		printf("Error : you entered a quantity superior to the stocks\n");
		exit(1);
	}
	if (apple.price <0 || orange.price <0 || banana.price <0){
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
	

int main (){
	checkScenario();
	
	int p1[2], p2[2], p3[2], p4[2], p5[2], p6[2];

	printf("Beginning of the scenario.\n");
	
	printf("Program. Creating the pipes.\n");
	pipeSucceed(p1);
	pipeSucceed(p2);
	pipeSucceed(p3);
	pipeSucceed(p4);
	pipeSucceed(p5);
	pipeSucceed(p6);
	
	printf("Program. Activating the non blocking mode for each pipe.\n");
	nonBlocking(p1);
	nonBlocking(p2);
	nonBlocking(p3);
	nonBlocking(p4);
	nonBlocking(p5);
	nonBlocking(p6);

	return 0;
}