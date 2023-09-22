//
// Created by ludovic.fanus@etu.umontpellier.fr on 22/09/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

// Définition des variables globales
int mon_numero = 0;              // Le numéro de ce processus Pconfig
int socket_fd;                   // Descripteur de fichier du socket pour Pconfig

// Structure pour stocker les informations de configuration de chaque processus Pi
typedef struct {
    int numero;
    char adresse_IP[20];  // Remplacez la taille en conséquence
    int port;
} InfoProcessus;

// Fonction pour créer et configurer le socket pour Pconfig
void configurer_socket(int port_local) {
    // Création du socket TCP pour Pconfig
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("Erreur lors de la création du socket pour Pconfig");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse locale pour Pconfig
    struct sockaddr_in adresse_locale;
    memset(&adresse_locale, 0, sizeof(adresse_locale));
    adresse_locale.sin_family = AF_INET;
    adresse_locale.sin_addr.s_addr = htonl(INADDR_ANY);
    adresse_locale.sin_port = htons(port_local);

    // Lier le socket à l'adresse locale
    if (bind(socket_fd, (struct sockaddr*)&adresse_locale, sizeof(adresse_locale)) == -1) {
        perror("Erreur lors de la liaison du socket à l'adresse locale pour Pconfig");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
}

// Fonction pour attendre que tous les processus Pi s'inscrivent et configurer l'anneau
void attendre_inscription_et_configurer_anneau(int n) {
    printf("Pconfig est en attente des inscriptions des processus Pi...\n");

    // Créez un tableau pour stocker les informations de configuration de chaque processus Pi
    InfoProcessus infos_pi[n];
    int pi_inscrits = 0;

    while (pi_inscrits < n) {
        // Acceptez les connexions entrantes des processus Pi
        struct sockaddr_in adresse_client;
        socklen_t taille_adresse = sizeof(adresse_client);
        int nouveau_socket = accept(socket_fd, (struct sockaddr*)&adresse_client, &taille_adresse);
        if (nouveau_socket == -1) {
            perror("Erreur lors de l'acceptation d'une connexion de processus Pi");
            continue;
        }

        // Recevez les informations de configuration de Pi
        InfoProcessus info_pi;
        recv(nouveau_socket, &info_pi, sizeof(InfoProcessus), 0);

        // Stockez les informations de configuration de Pi dans le tableau
        infos_pi[pi_inscrits] = info_pi;
        pi_inscrits++;

        // Affichez un message pour indiquer que Pi s'est inscrit
        printf("P%d s'est inscrit avec l'adresse IP %s et le port %d.\n", info_pi.numero, info_pi.adresse_IP, info_pi.port);

        // Fermez la connexion avec Pi
        close(nouveau_socket);
    }

    // Maintenant que tous les processus Pi se sont inscrits, configurez l'anneau en utilisant les informations
    // stockées dans le tableau infos_pi et établissez les connexions entre eux pour former l'anneau.
    // Vous devez implémenter cette partie en utilisant les informations dans le tableau.

    printf("La configuration de l'anneau est terminée. Pconfig se termine.\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Utilisation : %s <numéro du processus>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_processus = atoi(argv[1]);

    // Configuration et création du socket pour Pconfig
    configurer_socket(num_processus);

    // Pconfig est prêt à recevoir les inscriptions et à configurer l'anneau
    attendre_inscription_et_configurer_anneau(n);

    // Pconfig n'a plus de rôle actif à jouer et peut se terminer
    close(socket_fd);

    return 0;
}
