# Livraisons : stocker, trier, charger

Programme en **C** qui gère un stock de colis et choisit lesquels mettre dans un camion.

Franklin VERSAYO — TP d'algorithmique

## Pourquoi le C

Rien n'est fourni d'avance : table de hachage, tris et programmation dynamique sont codés à la main.
Chaque coût en temps et en mémoire est donc visible dans le code, ce qui est justement ce qu'on
cherche à analyser. Le C permet en plus de serrer la mémoire au plus près (la table de la
programmation dynamique tient sur un bit par case).

## Compilation et exécution

```bash
gcc -std=c11 -O2 -Wall -o livraisons src/*.c
./livraisons
```

Au lancement, les 5 colis de l'exemple du sujet sont déjà chargés.

## Organisation du dépôt

| Fichier                        | Contenu                                                                               |
| ------------------------------ | ------------------------------------------------------------------------------------- |
| `src/colis.h`, `src/colis.c`   | le type `Colis` et la collection (tableau extensible indexé par une table de hachage) |
| `src/tri.h`, `src/tri.c`       | tri fusion pour poids, valeur et destination ; tri par comptage pour la priorité      |
| `src/camion.h`, `src/camion.c` | sélection des colis à charger par programmation dynamique                             |
| `src/main.c`                   | menu, statistiques, banc d'essai des performances                                     |
| `rapport.pdf`                  | rapport : justification des choix, complexités, mesures, réponses aux questions       |

## Algorithmes en bref

| Fonctionnalité                                                   | Technique                                                  | Coût en temps   | Coût en mémoire |
| ---------------------------------------------------------------- | ---------------------------------------------------------- | --------------- | --------------- |
| Ajout d'un colis                                                 | tableau qui double de taille + table de hachage            | O(1) amorti     | O(n)            |
| Recherche par identifiant                                        | hachage, adressage ouvert et sondage linéaire              | O(1) en moyenne | O(n)            |
| Classement par poids, valeur ou destination (dans les deux sens) | tri fusion sur un tableau d'indices                        | O(n log n)      | O(n)            |
| Classement par priorité                                          | tri par comptage (5 niveaux)                               | O(n)            | O(n)            |
| Chargement du camion                                             | sac à dos 0/1 en programmation dynamique, résultat optimal | O(n·C)          | O(n·C) bits     |

## À savoir

- Quand on demande de tenir compte de la priorité au moment de préparer le camion (option 5), la
  quantité maximisée devient `valeur × (priorité + 3)` : à valeur égale, un colis de priorité 5 pèse
  deux fois plus qu'un colis de priorité 1. La capacité du camion n'est jamais dépassée.
- L'option 7 affiche les statistiques et propose un banc d'essai sur 100, 1 000, 10 000 et
  100 000 colis.
