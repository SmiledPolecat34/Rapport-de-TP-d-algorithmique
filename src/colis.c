#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "colis.h"

void *allouer(size_t taille)
{
    void *p = calloc(taille ? taille : 1, 1);
    if (!p) {
        fprintf(stderr, "Memoire insuffisante\n");
        exit(1);
    }
    return p;
}

/* ---------- Collection : tableau dynamique + table de hachage ---------- */

/* Hachage multiplicatif de Knuth : on garde les bits de poids fort de id * 2654435761. */
static int hacher(int id, int bits)
{
    return (int)((unsigned)id * 2654435761u >> (32 - bits));
}

/* Case contenant id, ou case vide ou l'inserer (sondage lineaire). */
static int trouver_case(const Entrepot *e, int id)
{
    int h = hacher(id, e->bits);
    while (e->table[h] != -1 && e->colis[e->table[h]].id != id)
        h = (h + 1) & ((1 << e->bits) - 1);
    return h;
}

static void creer_table(Entrepot *e, int bits)
{
    free(e->table);
    e->bits = bits;
    e->table = allouer(sizeof(int) << bits);
    memset(e->table, -1, sizeof(int) << bits);
    for (int i = 0; i < e->n; i++)
        e->table[trouver_case(e, e->colis[i].id)] = i;
}

void entrepot_init(Entrepot *e)
{
    memset(e, 0, sizeof *e);
    creer_table(e, 4);
}

void entrepot_liberer(Entrepot *e)
{
    free(e->colis);
    free(e->table);
}

int ajouter_colis(Entrepot *e, Colis c)
{
    if (rechercher_colis(e, c.id))
        return -1;
    if (e->n == e->capacite) { /* tableau plein : on double sa taille -> O(1) amorti */
        e->capacite = e->capacite ? 2 * e->capacite : 16;
        e->colis = realloc(e->colis, e->capacite * sizeof(Colis));
        if (!e->colis) {
            fprintf(stderr, "Memoire insuffisante\n");
            exit(1);
        }
    }
    if (2 * (e->n + 1) > 1 << e->bits) /* table remplie a moitie : on la double */
        creer_table(e, e->bits + 1);
    e->colis[e->n] = c;
    e->table[trouver_case(e, c.id)] = e->n;
    e->n++;
    return 0;
}

Colis *rechercher_colis(const Entrepot *e, int id)
{
    int i = e->table[trouver_case(e, id)];
    return i == -1 ? NULL : &e->colis[i];
}

int recherche_lineaire(const Entrepot *e, int id)
{
    for (int i = 0; i < e->n; i++)
        if (e->colis[i].id == id)
            return i;
    return -1;
}

/* Le colis ordre[i] passe en position i ; les indices ayant change, on reconstruit la table. */
void reordonner(Entrepot *e, const int *ordre)
{
    Colis *trie = allouer(e->capacite * sizeof(Colis));
    for (int i = 0; i < e->n; i++)
        trie[i] = e->colis[ordre[i]];
    free(e->colis);
    e->colis = trie;
    creer_table(e, e->bits);
}
