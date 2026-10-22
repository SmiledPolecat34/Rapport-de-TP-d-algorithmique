#ifndef CAMION_H
#define CAMION_H

#include "colis.h"

/* Choix optimal des colis pour un camion de capacite donnee (kg), avec ou sans priorite.
 * Remplit charges[] (n indices au plus) et retourne le nombre de colis charges. */
int preparer_camion(const Entrepot *e, int capacite, int avec_priorite, int *charges);

#endif
