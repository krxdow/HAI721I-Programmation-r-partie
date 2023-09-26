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
#include <errno.h>
#include <limits.h>

#define PORT_PAR_DEFAUT 4445

// Définition des variables globales
int mon_numero = 0;              // Le numéro de ce processus Pconfig
int socket_fd;

char buffer[256]; // Tableau de caractaire de reception du port
size_t port_voisin;

// Structure pour stocker les informations de configuration de chaque processus Pi
typedef struct {
    int numero;
    char adresse_IP[20];  // Remplacez la taille en conséquence
    int port;
} InfoProcessus;


struct sockaddr_in servaddr = {0};




void server(){
    int sockfd, new_sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Créez le socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Erreur lors de la création du socket");
        exit(1);
    }

    // Initialisez la structure du serveur
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Laissez le système choisir l'adresse IP
    server_addr.sin_port = 0; // Laissez le système attribuer automatiquement le port

    // Liez le socket au port et à l'adresse IP
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur lors de la liaison du socket");
        close(sockfd);
        exit(1);
    }

    // Obtenez le numéro de port attribué automatiquement par le system
    getsockname(sockfd, (struct sockaddr*)&server_addr, &client_len);
    int local_port = ntohs(server_addr.sin_port);
    printf("Le serveur  local écoute sur le port : %d\n", local_port);


    // Convertissez le numéro de port local en une chaîne de caractères
    char bufferPort[256];
    snprintf(bufferPort, sizeof(bufferPort), "%d", local_port);

/*
    // Envoyez le numéro de port au au serveur principale
    if (send(socket_fd, bufferPort, strlen(bufferPort), 0) == -1) {
        perror("Erreur lors de l'envoi du numéro de port au client");
    }
*/

    // Envoi de données au serveur principal
    const char* message = "Message depuis le client UDP.";
   int bytes_received =sendto(socket_fd, bufferPort, strlen(bufferPort), 0, (struct sockaddr*)&servaddr, client_len);
    if (bytes_received == -1) {
        perror("Erreur lors de la réception des données");
       // break;
    }

    // Réception de la réponse du serveur
    bytes_received = recvfrom(socket_fd, buffer, strlen(buffer), 0, NULL, NULL);
    if (bytes_received == -1) {
        perror("Erreur lors de la réception de la réponse du serveur");
        close(sockfd);
        exit(1);
    }
    printf("reponse srv : %s",buffer);



    // Mettez le serveur en mode écoute
    if (listen(sockfd, 5) == -1) {
        perror("Erreur lors de la mise en écoute du socket");
        close(sockfd);
        exit(1);
    }

    // Acceptez la connexion entrante
/*
    new_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
    if (new_sockfd == -1) {
        perror("Erreur lors de l'acceptation de la connexion");
        close(sockfd);
        exit(1);
    }
*/

    // Traitez la connexion

    close(new_sockfd);
    close(sockfd);




}







// Fonction pour se connecter au serveur principal
void connection(int port_utilise) {
    // Création du socket
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // Connexion au serveur
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(port_utilise);

/*
    if (connect(socket_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
        perror("Erreur lors de la connexion au serveur");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
*/

    // Obtenir des informations sur le client
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    // Utilisez getsockname pour obtenir les informations du socket local
    if (getsockname(socket_fd, (struct sockaddr*)&client_addr, &client_addr_len) == -1) {
        perror("Erreur lors de l'obtention des informations du socket local");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("\nConnecté au serveur principal\nPort local du client : %d\n", ntohs(client_addr.sin_port));

    // le srv envoie message quand n_clients = n
    ssize_t n = 0;
    // Réception du message


/*
    // Réception de la réponse du serveur
    int bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
    if (bytes_received == -1) {
        perror("Erreur lors de la réception de la réponse du serveur");
        close(sockfd);
        exit(1);
    }

*/





//serveur local
  server();


    n = recv(socket_fd, buffer, sizeof(buffer) - 1, 0);
    if (n <= 0) {
        // Fermeture de la connexion
        if (n == 0) {
            printf("Serveur déconnecté.\n");
        } else {
            perror("Erreur lors de la réception du message");
        }
        // close(socket_fd);
        // Exit seulement si vous voulez quitter le programme ici
        // Sinon, vous pouvez continuer à recevoir des messages
        // exit(EXIT_SUCCESS);
    } else {
        buffer[n] = '\0';
        // Affichage du message
        printf("Port voisin donné par le serveur : %s\n", buffer);
        // Conversion du port voisin en entier
        char *endptr; // Pointeur vers la fin de la conversion
        long port_long = strtol(buffer, &endptr, 10);
        if (endptr == buffer) {
            // La conversion a échoué
            printf("Erreur : la conversion du port a échoué.\n");
        } else {
            // Utilisez port_long comme numéro de port
            int port_voisin = (int)port_long;
            printf("Port voisin converti en entier : %d\n", port_voisin);
            // Vous pouvez maintenant utiliser port_voisin comme numéro de port dans votre programme
        }
    }
}









// Fonction pour se connecter au voisin avec le port donné
void connect_to_neighbor(size_t port) {

    struct sockaddr_in voisinAddr;
    voisinAddr.sin_family = AF_INET;
    voisinAddr.sin_port = htons(port);
    voisinAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
   int socketfd_co;

    if (connect(socketfd_co, (struct sockaddr *)&voisinAddr, sizeof(voisinAddr)) == -1) {
        perror("Erreur lors de la connexion au voisin");
        close(socketfd_co);
        exit(EXIT_FAILURE);
    }
    printf("[+]Connected to Neighbor.\n");


}


// Fonction pour la boucle de réception des messages du voisin
void receive_neighbor_messages() {

    // Lier le socket à l'adresse locale
    if (bind(socket_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
        perror("Erreur lors de la liaison du socket à l'adresse locale pour Pconfig");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    printf("Client passage en mode ecoute:\n");
    printf("Adresse IP : %s\n", inet_ntoa(servaddr.sin_addr));
    printf("Port : %hu\n", ntohs(servaddr.sin_port));

    // Mettre le socket en mode écoute
    if (listen(socket_fd, 5) == -1) {
        perror("Erreur lors de la mise en écoute du socket");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    printf("[+]Listening...\n");


    while (1) {
        ssize_t n = recv(socket_fd, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            perror("Erreur lors de la réception du message du voisin");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }
        buffer[n] = '\0';

        printf("Message du voisin : %s\n", buffer);
    }
}


int main(int argc, char *argv[]) {
    int port = -1; // Valeur par défaut si l'option -p n'est pas fournie
    int opt;
    close(socket_fd);//liber le port pour relancer

    // Utilisation de getopt pour gérer les arguments
    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p': {
                char *endptr;
                errno = 0; // Réinitialise errno avant l'appel à strtol
                long int port_long = strtol(optarg, &endptr, 10);

                // Vérifiez les erreurs de conversion
                if ((errno == ERANGE && (port_long == LONG_MAX || port_long == LONG_MIN)) || (errno != 0 && port_long == 0)) {
                    fprintf(stderr, "Erreur : conversion de l'argument de l'option -p en entier échouée (dépassement de capacité ou argument non numérique).\n");
                    return 1;
                }

                if (endptr == optarg || *endptr != '\0') {
                    fprintf(stderr, "Erreur : l'argument de l'option -p n'est pas un entier valide.\n");
                    return 1;
                }

                port = (int)port_long;

                if (port <= 0) {
                    fprintf(stderr, "Erreur : l'argument de l'option -p doit être un entier positif.\n");
                    return 1;
                }
                break;
            }
            default:
                fprintf(stderr, "Utilisation : %s [-p <port>]\n", argv[0]);
                return 1;
        }
    }
    if (port == -1) {
        printf("Aucun port spécifié. Utilisation du port par défaut : %d pour se connecter au serveur.\n", PORT_PAR_DEFAUT);
    }
    int port_utilise = (port == -1) ? PORT_PAR_DEFAUT : port;

    // Appel de la fonction de connexion au serveur
    connection(port_utilise);

 //   connect_to_neighbor(port_voisin);
//      receive_neighbor_messages();
    close(socket_fd);


    return 0;
}
