#ifndef TRI_H
#define TRI_H

#include "colis.h"

typedef enum { POIDS, VALEUR, PRIORITE, DESTINATION } Critere;

/* Remplit ordre[] avec les n indices des colis tries selon le critere (destination : ordre
 * alphabetique), croissant si croissant != 0, decroissant sinon. */
void trier_colis(const Entrepot *e, Critere critere, int croissant, int *ordre);

#endif
