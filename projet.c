#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/* Choose the desired scenario by putting or removing // : */

	/* Server : */
char *server = "sw1";
//char *server = sw2;
//char *server = sw3;

	/* Buyer : */
char *buyer = "Laure";
//char *buyer = "Philippe";
//char *buyer = "Paul";

	/* DeliveryDriver : */
char *deliveryDriver = "Xavier";
//char *deliveryDriver = "Michel";
//char *deliveryDriver = "France";

/* Choose the desired scenario by putting the quantities and prices : */

typedef struct {
	float quantity;			/* the quantity, in kg, of apples the buyer wants to buy */
	int stock;				/* the number of crates of apples */
	float price;			/* the price, in euros, of a kg of apples */
}Apple;

typedef struct {
	float quantity;			/* the quantity, in kg, of oranges the buyer wants to buy */
	int stock;				/* the number of crates of oranges */
	float price;			/* the price, in euros, of a kg of oranges */
}Orange;

typedef struct {
	float quantity;			/* the quantity, in kg, of bananas the buyer wants to buy */
	int stock;				/* the number of crates of bananas */
	float price;			/* the price, in euros, of a kg of bananas */
}Banana;

	/* Quantities, stocks and prices of the articles : */

Apple apple = {1, 2, 3};

Orange orange = {1, 2, 3};

Banana banana = {1, 2, 3};

	

int main (){


	return 0;
}