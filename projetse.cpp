#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 10

// Structure pour les arguments du thread producteur
typedef struct {
    int row;
    int n1, m1, n2, m2;
    double *mat_B, *mat_C, *tampon;
} ProducerArgs;

// Structure pour les arguments du thread consommateur
typedef struct {
    int row;
    int m2;
    double *mat_A, *tampon;
} ConsumerArgs;

// Fonction de calcul de la ligne de la matrice résultante A
void *producteur(void *args) {
    ProducerArgs *pargs = (ProducerArgs *)args;

    int row = pargs->row;
    int n1 = pargs->n1;
    int m1 = pargs->m1;
    int n2 = pargs->n2;
    int m2 = pargs->m2;

    for (int i = 0; i < m2; ++i) {
        double result = 0.0;
        for (int j = 0; j < m1; ++j) {
            result += pargs->mat_B[row * m1 + j] * pargs->mat_C[j * m2 + i];
        }
        pargs->tampon[row * m2 + i] = result;
    }

    pthread_exit(NULL);
}

// Fonction de consommation et placement dans la matrice résultante A
void *consommateur(void *args) {
    ConsumerArgs *cargs = (ConsumerArgs *)args;

    int row = cargs->row;
    int m2 = cargs->m2;

    for (int i = 0; i < m2; ++i) {
        cargs->mat_A[row * m2 + i] = cargs->tampon[row * m2 + i];
    }

    pthread_exit(NULL);
}

int main() {
    int n1, m1, n2, m2;

    // Demander à l'utilisateur les dimensions des matrices
    printf("Entrez le nombre de lignes de la matrice B : ");
    scanf("%d", &n1);
    printf("Entrez le nombre de colonnes de la matrice B : ");
    scanf("%d", &m1);

    printf("Entrez le nombre de lignes de la matrice C : ");
    scanf("%d", &n2);
    printf("Entrez le nombre de colonnes de la matrice C : ");
    scanf("%d", &m2);

    // Générer des matrices B et C avec des valeurs fournies par l'utilisateur
    double *mat_B = (double *)malloc(n1 * m1 * sizeof(double));
    double *mat_C = (double *)malloc(m1 * m2 * sizeof(double));

    printf("Entrez les valeurs de la matrice B (%dx%d) :\n", n1, m1);
    for (int i = 0; i < n1; ++i) {
        for (int j = 0; j < m1; ++j) {
            printf("B[%d][%d] : ", i, j);
            scanf("%lf", &mat_B[i * m1 + j]);
        }
    }

    printf("Entrez les valeurs de la matrice C (%dx%d) :\n", m1, m2);
    for (int i = 0; i < m1; ++i) {
        for (int j = 0; j < m2; ++j) {
            printf("C[%d][%d] : ", i, j);
            scanf("%lf", &mat_C[i * m2 + j]);
        }
    }

    // Initialiser la matrice résultante A avec des zéros
    double *mat_A = (double *)malloc(n1 * m2 * sizeof(double));

    // Définir la taille du tampon
    int N = n1;

    // Initialiser le tampon avec des zéros
    double *tampon = (double *)malloc(N * m2 * sizeof(double));

    // Liste pour stocker les threads
    pthread_t threads[MAX_THREADS];

    // Créer et démarrer les threads producteurs
    for (int i = 0; i < n1; ++i) {
        ProducerArgs *args = (ProducerArgs *)malloc(sizeof(ProducerArgs));
        args->row = i;
        args->n1 = n1;
        args->m1 = m1;
        args->n2 = n2;
        args->m2 = m2;
        args->mat_B = mat_B;
        args->mat_C = mat_C;
        args->tampon = tampon;

        pthread_create(&threads[i], NULL, producteur, (void *)args);
    }

    // Attendre que tous les threads producteurs aient terminé
    for (int i = 0; i < n1; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Réinitialiser la liste des threads pour les consommateurs
    for (int i = 0; i < n1; ++i) {
        ConsumerArgs *args = (ConsumerArgs *)malloc(sizeof(ConsumerArgs));
        args->row = i;
        args->m2 = m2;
        args->mat_A = mat_A;
        args->tampon = tampon;

        pthread_create(&threads[i], NULL, consommateur, (void *)args);
    }

    // Attendre que tous les threads consommateurs aient terminé
    for (int i = 0; i < n1; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Afficher les matrices résultantes (optionnel)
    printf("Matrice B :\n");
    for (int i = 0; i < n1; ++i) {
        for (int j = 0; j < m1; ++j) {
            printf("%lf ", mat_B[i * m1 + j]);
        }
        printf("\n");
    }

    printf("\nMatrice C :\n");
    for (int i = 0; i < m1; ++i) {
        for (int j = 0; j < m2; ++j) {
            printf("%lf ", mat_C[i * m2 + j]);
        }
        printf("\n");
    }

    printf("\nMatrice résultante A :\n");
    for (int i = 0; i < n1; ++i) {
        for (int j = 0; j < m2; ++j) {
            printf("%lf ", mat_A[i * m2 + j]);
        }
        printf("\n");
    }

    // Libérer la mémoire
    free(mat_B);
    free(mat_C);
    free(mat_A);
    free(tampon);

    return 0;
}

