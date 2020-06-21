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
char *server = sw1;
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

typedef struct apple Apple{
	float quantity;			/* the quantity, in kg, of apples the buyer wants to buy */
	int stock;				/* the number of crates of apples */
	float price;			/* the price, in euros, of a kg of apples */
}

typedef struct orange Orange{
	float quantity;			/* the quantity, in kg, of oranges the buyer wants to buy */
	int stock;				/* the number of crates of oranges */
	float price;			/* the price, in euros, of a kg of oranges */
}

typedef struct banana Banana{
	float quantity;			/* the quantity, in kg, of bananas the buyer wants to buy */
	int stock;				/* the number of crates of bananas */
	float price;			/* the price, in euros, of a kg of bananas */
}

	/* List, stocks and prices of the articles : */

Apple apple;
apple.quantity = 1;
apple.stock = 2;
apple.price = 3;

Orange orange;
orange.quantity = 1;
orange.stock = 2;
orange.price = 3;

Banana banana;
banana.quantity = 1;
banana.stock = 2;
banana.price = 3;

	

int main (){



	return 0;
}