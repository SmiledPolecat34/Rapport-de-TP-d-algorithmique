#include <stdlib.h>

#include "camion.h"

/* Chargement du camion : sac a dos 0/1 par programmation dynamique. */

/* Score maximise. Avec priorite : valeur * (priorite + 3), soit valeur x1 (priorite 1)
 * a valeur x2 (priorite 5), a un facteur 4 pres ; en entiers pour des comparaisons exactes. */
static long long score(const Colis *c, int avec_priorite)
{
    return avec_priorite ? (long long)c->valeur * (c->priorite + 3) : c->valeur;
}

/* meilleur[c] = meilleur score avec une capacite c et les colis deja traites. Pour chaque colis
 * (poids w, score s), c va de C a w a rebours (chaque colis n'est pris qu'une fois) :
 *     meilleur[c] = max(meilleur[c], meilleur[c - w] + s)
 * Le bit pris[i][c] note que le colis i a ameliore meilleur[c] ; on reconstruit la solution en
 * remontant les colis du dernier au premier. Temps O(n * C), espace O(n * C) bits + O(C).
 * Remplit charges[] (n indices au plus) et retourne le nombre de colis charges. */
int preparer_camion(const Entrepot *e, int capacite, int avec_priorite, int *charges)
{
    size_t largeur = (size_t)capacite + 1;
    long long *meilleur = allouer(largeur * sizeof(long long));
    unsigned char *pris = allouer(((size_t)e->n * largeur + 7) / 8);

    for (int i = 0; i < e->n; i++) {
        int w = e->colis[i].poids;
        long long s = score(&e->colis[i], avec_priorite);
        for (int c = capacite; c >= w; c--)
            if (meilleur[c - w] + s > meilleur[c]) {
                meilleur[c] = meilleur[c - w] + s;
                size_t bit = i * largeur + c;
                pris[bit / 8] |= 1 << bit % 8;
            }
    }

    int nb = 0, c = capacite;
    for (int i = e->n - 1; i >= 0; i--) {
        size_t bit = i * largeur + c;
        if (pris[bit / 8] >> bit % 8 & 1) {
            charges[nb++] = i;
            c -= e->colis[i].poids;
        }
    }
    free(meilleur);
    free(pris);
    return nb;
}
