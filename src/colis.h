#ifndef COLIS_H
#define COLIS_H

#include <stddef.h>

typedef struct {
    int id, poids, valeur, priorite; /* poids en kg (>= 1), priorite de 1 (faible) a 5 (urgente) */
    char destination[32];
} Colis;

typedef struct {
    Colis *colis;   /* tableau dynamique, dans l'ordre d'ajout */
    int n, capacite;
    int *table;     /* table de hachage : id -> indice dans colis[], -1 = case vide */
    int bits;       /* la table contient 2^bits cases */
} Entrepot;

void *allouer(size_t taille);

void entrepot_init(Entrepot *e);
void entrepot_liberer(Entrepot *e);
int ajouter_colis(Entrepot *e, Colis c);              /* 0 si ajoute, -1 si l'id existe deja */
Colis *rechercher_colis(const Entrepot *e, int id);   /* O(1) en moyenne, NULL si absent */
int recherche_lineaire(const Entrepot *e, int id);    /* O(n), sert de comparaison */
void reordonner(Entrepot *e, const int *ordre);       /* range les colis dans l'ordre donne, O(n) */

#endif
