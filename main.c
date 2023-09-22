#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

// Définition des variables globales
int mon_numero;                   // Le numéro de ce processus
int taille_de_l_anneau = 1;      // Initialisation à 1
char* adresse_voisin_suivant;    // Adresse IP du voisin suivant
int port_voisin_suivant;         // Port du voisin suivant
int socket_fd;                   // Descripteur de fichier du socket

// Fonction pour envoyer un message REQ
void envoyer_REQ(int vers_processus) {
    char message[256];
    sprintf(message, "REQ %d %d", mon_numero, taille_de_l_anneau);
    send(socket_fd, message, strlen(message), 0);
}

// Fonction pour recevoir un message REQ
void recevoir_REQ() {
    char message[256];
    recv(socket_fd, message, sizeof(message), 0);
    int i, taille_partielle;
    sscanf(message, "REQ %d %d", &i, &taille_partielle);
    if (i == mon_numero) {
        // Le message a fait le tour complet de l'anneau
        taille_de_l_anneau = taille_partielle;
    } else {
        envoyer_REQ(0);  // Transmettre le message REQ au voisin suivant
    }
}

// Fonction principale pour chaque processus Pi
void processus_principal() {
    while (1) { // Boucle infinie
        if (mon_numero == 1) {
            envoyer_REQ(0); // Déclencher la transmission du message REQ
            recevoir_REQ(); // Attendre la réponse pour collecter la taille de l'anneau
            printf("La taille de l'anneau est %d\n", taille_de_l_anneau);
        } else {
            recevoir_REQ(); // Attendre de recevoir un message REQ
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Utilisation: %s <numéro> <adresse_IP_voisin_suivant> <port_voisin_suivant> <port_local>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Récupération des paramètres passés en ligne de commande
    mon_numero = atoi(argv[1]);
    adresse_voisin_suivant = argv[2];
    port_voisin_suivant = atoi(argv[3]);
    int port_local = atoi(argv[4]);

    // Création du socket TCP
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse locale
    struct sockaddr_in adresse_locale;
    memset(&adresse_locale, 0, sizeof(adresse_locale));
    adresse_locale.sin_family = AF_INET;
    adresse_locale.sin_addr.s_addr = htonl(INADDR_ANY);
    adresse_locale.sin_port = htons(port_local);

    // Lier le socket à l'adresse locale
    if (bind(socket_fd, (struct sockaddr*)&adresse_locale, sizeof(adresse_locale)) == -1) {
        perror("Erreur lors de la liaison du socket à l'adresse locale");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    // Connexion au voisin suivant
    struct sockaddr_in adresse_voisin;
    memset(&adresse_voisin, 0, sizeof(adresse_voisin));
    adresse_voisin.sin_family = AF_INET;
    adresse_voisin.sin_addr.s_addr = inet_addr(adresse_voisin_suivant);
    adresse_voisin.sin_port = htons(port_voisin_suivant);

    if (connect(socket_fd, (struct sockaddr*)&adresse_voisin, sizeof(adresse_voisin)) == -1) {
        perror("Erreur lors de la connexion au voisin suivant");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    // Lancement du processus principal
    processus_principal();

    // Fermeture du socket lorsque vous avez terminé
    close(socket_fd);

    return 0;
}
