#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "camion.h"
#include "colis.h"
#include "tri.h"

static int lire_entier(const char *message, int min, int max)
{
    char ligne[64];
    for (;;) {
        printf("%s", message);
        if (!fgets(ligne, sizeof ligne, stdin))
            exit(0);
        char *fin;
        long v = strtol(ligne, &fin, 10);
        if (fin != ligne && (*fin == '\n' || *fin == '\0') && v >= min && v <= max)
            return (int)v;
        printf("  Entier entre %d et %d attendu.\n", min, max);
    }
}

static void afficher(const Colis *c)
{
    printf("%8d | %5d | %6d | %8d | %s\n", c->id, c->poids, c->valeur, c->priorite, c->destination);
}

static void entete(void)
{
    printf("%8s | %5s | %6s | %8s | %s\n", "ID", "Poids", "Valeur", "Priorite", "Destination");
}

static void ajouter(Entrepot *e)
{
    Colis c;
    c.id = lire_entier("  Identifiant : ", 0, 2000000000);
    while (rechercher_colis(e, c.id)) {
        printf("  Cet identifiant existe deja.\n");
        c.id = lire_entier("  Identifiant : ", 0, 2000000000);
    }
    c.poids = lire_entier("  Poids (kg) : ", 1, 1000000);
    c.valeur = lire_entier("  Valeur : ", 0, 1000000);
    c.priorite = lire_entier("  Priorite (1 a 5) : ", 1, 5);
    printf("  Destination : ");
    if (!fgets(c.destination, sizeof c.destination, stdin))
        exit(0);
    if (!strchr(c.destination, '\n')) /* nom trop long : on ignore la fin de la ligne */
        while (getchar() != '\n' && !feof(stdin))
            ;
    c.destination[strcspn(c.destination, "\n")] = '\0';
    printf(ajouter_colis(e, c) == 0 ? "  Colis ajoute.\n" : "  Cet identifiant existe deja.\n");
}

static void organiser(Entrepot *e)
{
    Critere critere = lire_entier("Trier par: \n1. poids \n2. valeur \n3. priorite \n4. destination\n-> ", 1, 4) - 1;
    int croissant = lire_entier("Ordre: \n1. croissant \n2. decroissant\n-> ", 1, 2) == 1;
    int *ordre = allouer(e->n * sizeof(int));
    trier_colis(e, critere, croissant, ordre);
    reordonner(e, ordre); /* l'option 1 affichera desormais cet ordre */
    free(ordre);
    entete();
    for (int i = 0; i < e->n; i++)
        afficher(&e->colis[i]);
}

static void statistiques(const Entrepot *e)
{
    long long poids = 0, valeur = 0;
    int par_priorite[6] = {0};
    for (int i = 0; i < e->n; i++) {
        poids += e->colis[i].poids;
        valeur += e->colis[i].valeur;
        par_priorite[e->colis[i].priorite]++;
    }
    printf("  %d colis, poids total %lld kg, valeur totale %lld\n", e->n, poids, valeur);
    for (int p = 5; p >= 1; p--)
        printf("  priorite %d : %d colis\n", p, par_priorite[p]);
}

/* Temps moyen (ms) d'une instruction, repetee jusqu'a cumuler 0,2 s (clock() est peu precis). */
#define MESURER(resultat, instruction)                                              \
    do {                                                                            \
        int r_ = 0;                                                                 \
        clock_t d_ = clock();                                                       \
        do { instruction; r_++; } while (clock() - d_ < CLOCKS_PER_SEC / 5);        \
        resultat = 1000.0 * (clock() - d_) / CLOCKS_PER_SEC / r_;                   \
    } while (0)

static void remplir(Entrepot *e, int n)
{
    entrepot_init(e);
    srand(42);
    for (int i = 0; i < n; i++) {
        Colis c = {i + 1, 1 + rand() % 30, 1 + rand() % 100, 1 + rand() % 5, "Paris"};
        ajouter_colis(e, c);
    }
}

static void mesures(void)
{
    const int tailles[] = {100, 1000, 10000, 100000};
    printf("\n  Temps en ms          |       100 |      1000 |     10000 |    100000\n");
    double t[6][4];
    for (int k = 0; k < 4; k++) {
        int n = tailles[k], trouves = 0;
        Entrepot e;
        int *tab = allouer(n * sizeof(int));
        MESURER(t[0][k], (remplir(&e, n), entrepot_liberer(&e)));
        remplir(&e, n);
        MESURER(t[1][k], for (int i = 0; i < 1000; i++) trouves += rechercher_colis(&e, 1 + (int)((long long)i * 7919 % n)) != NULL);
        MESURER(t[2][k], for (int i = 0; i < 1000; i++) trouves += recherche_lineaire(&e, 1 + (int)((long long)i * 7919 % n)));
        MESURER(t[3][k], trier_colis(&e, POIDS, 0, tab));
        MESURER(t[4][k], trier_colis(&e, PRIORITE, 0, tab));
        MESURER(t[5][k], preparer_camion(&e, 50, 1, tab));
        if (trouves < 0) /* utilise le resultat pour que le compilateur garde les boucles */
            printf("!");
        free(tab);
        entrepot_liberer(&e);
    }
    const char *noms[] = {"ajout des n colis", "1000 rech. hachage", "1000 rech. lineaires",
                          "tri fusion (poids)", "tri comptage (prio)", "camion 50 kg (DP)"};
    for (int o = 0; o < 6; o++)
        printf("  %-20s | %9.3f | %9.3f | %9.3f | %9.3f\n", noms[o], t[o][0], t[o][1], t[o][2], t[o][3]);
}

int main(void)
{
    Entrepot e;
    Colis exemple[] = {{101, 8, 20, 2, "Paris"}, {102, 3, 10, 5, "Lille"}, {103, 12, 40, 1, "Lyon"},
                       {104, 5, 25, 4, "Paris"}, {105, 2, 15, 3, "Lille"}};
    entrepot_init(&e);
    for (int i = 0; i < 5; i++)
        ajouter_colis(&e, exemple[i]);

    int *charges = NULL, nb_charges = 0, capacite = 0;
    for (;;) {
        printf("\n===== GESTION DES LIVRAISONS =====\n"
               "1. Afficher les colis\n2. Ajouter un colis\n3. Rechercher un colis\n"
               "4. Organiser les colis\n5. Preparer un camion\n6. Afficher le chargement\n"
               "7. Afficher les statistiques\n0. Quitter\n");
        int choix = lire_entier("Choix : ", 0, 7);
        if (choix == 0)
            break;
        if (choix == 1) {
            entete();
            for (int i = 0; i < e.n; i++)
                afficher(&e.colis[i]);
        } else if (choix == 2) {
            ajouter(&e);
        } else if (choix == 3) {
            Colis *c = rechercher_colis(&e, lire_entier("  Identifiant : ", 0, 2000000000));
            if (c) {
                entete();
                afficher(c);
            } else {
                printf("  Colis introuvable.\n");
            }
        } else if (choix == 4) {
            organiser(&e);
        } else if (choix == 5) {
            capacite = lire_entier("  Capacite du camion (kg) : ", 1, 100000);
            int avec_priorite = lire_entier("  Tenir compte de la priorite ? (1 oui, 0 non) : ", 0, 1);
            free(charges);
            charges = allouer(e.n * sizeof(int));
            nb_charges = preparer_camion(&e, capacite, avec_priorite, charges);
            for (int i = 0; i < nb_charges; i++) /* on garde les identifiants, stables apres un tri */
                charges[i] = e.colis[charges[i]].id;
            printf("  %d colis charges (option 6 pour le detail).\n", nb_charges);
        } else if (choix == 6) {
            int poids = 0, valeur = 0;
            entete();
            for (int i = 0; i < nb_charges; i++) {
                const Colis *c = rechercher_colis(&e, charges[i]);
                afficher(c);
                poids += c->poids;
                valeur += c->valeur;
            }
            printf("  Total : %d / %d kg, valeur %d\n", poids, capacite, valeur);
        } else {
            statistiques(&e);
            if (lire_entier("  Mesurer les performances (100 a 100 000 colis) ? (1 oui, 0 non) : ", 0, 1))
                mesures();
        }
    }
    free(charges);
    entrepot_liberer(&e);
    return 0;
}
