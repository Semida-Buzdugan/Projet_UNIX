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
	float quantity;			/* the quantity, in kg, of the fruit the buyer wants to buy */
	int stock;				/* the number of crates of the fruit */
	float price;			/* the price, in euros, of a kg of the fruit */
}Fruit;

	/* Quantities, stocks and prices of the articles : */

Fruit apple = {1, 2, 3};

Fruit orange = {1, 2, 3};

Fruit banana = {1, 2, 3};

	

int main (){
	printf("Helllo world!");

	return 0;
}