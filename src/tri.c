#include <stdlib.h>
#include <string.h>

#include "tri.h"

typedef struct {
    const Entrepot *e;
    Critere critere;
    int croissant;
} Tri;

/* Le colis a doit-il passer avant le colis b ? En cas d'egalite oui (tri stable). */
static int avant(const Tri *t, int a, int b)
{
    const Colis *x = &t->e->colis[a], *y = &t->e->colis[b];
    int diff = t->critere == DESTINATION ? strcmp(x->destination, y->destination)
             : t->critere == POIDS       ? x->poids - y->poids
                                         : x->valeur - y->valeur;
    return t->croissant ? diff <= 0 : diff >= 0;
}

/* Tri fusion stable de ordre[debut..fin[ : on trie chaque moitie, puis on les fusionne.
 * O(n log n) comparaisons (O(n log n * L) pour des chaines de longueur L), tampon O(n). */
static void tri_fusion(const Tri *t, int *ordre, int *tmp, int debut, int fin)
{
    if (fin - debut < 2)
        return;
    int milieu = (debut + fin) / 2, i = debut, j = milieu, k = debut;
    tri_fusion(t, ordre, tmp, debut, milieu);
    tri_fusion(t, ordre, tmp, milieu, fin);
    while (i < milieu && j < fin)
        tmp[k++] = avant(t, ordre[i], ordre[j]) ? ordre[i++] : ordre[j++];
    while (i < milieu)
        tmp[k++] = ordre[i++];
    while (j < fin)
        tmp[k++] = ordre[j++];
    memcpy(ordre + debut, tmp + debut, (fin - debut) * sizeof(int));
}

void trier_colis(const Entrepot *e, Critere critere, int croissant, int *ordre)
{
    if (critere == PRIORITE) {
        /* Tri par comptage : la priorite ne prend que 5 valeurs -> O(n). */
        int debut[6] = {0};
        for (int i = 0; i < e->n; i++) {
            int rang = croissant ? e->colis[i].priorite - 1 : 5 - e->colis[i].priorite;
            debut[rang + 1]++;
        }
        for (int r = 1; r < 6; r++)
            debut[r] += debut[r - 1];
        for (int i = 0; i < e->n; i++) {
            int rang = croissant ? e->colis[i].priorite - 1 : 5 - e->colis[i].priorite;
            ordre[debut[rang]++] = i;
        }
        return;
    }
    Tri t = {e, critere, croissant};
    int *tmp = allouer(e->n * sizeof(int));
    for (int i = 0; i < e->n; i++)
        ordre[i] = i;
    tri_fusion(&t, ordre, tmp, 0, e->n);
    free(tmp);
}
