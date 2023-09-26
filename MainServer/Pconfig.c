//
// Created by dow on 24/09/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT_PAR_DEFAUT 4445


// Définition des variables globales
int mon_numero = 0;              // Le numéro de ce processus Pconfig
int socket_fd;                   // Descripteur de fichier du socket pour Pconfig



// Fonction pour envoyer le port du voisin au client
void envoyer_port_voisin(int clientfd, int *ports, int n, int mon_numero) {
    // Vérifier s'il y a au moins deux clients pour envoyer le port du voisin
    if (n >= 2) {
        int port_voisin;
        if (mon_numero == n) {
            // Si nous sommes le dernier client, le voisin est le premier client
            port_voisin = ports[0];
        } else {
            // Le voisin est le client suivant dans le tableau
            port_voisin = ports[mon_numero];
        }

        // Envoyer le port du voisin au client
        send(clientfd, &port_voisin, sizeof(port_voisin), 0);
    }
}








// Fonction pour créer et configurer le socket pour Pconfig
void configurer_socket(int port_local) {


    struct sockaddr_in adresse_locale,client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024];


    // Création du socket TCP pour Pconfig
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1) {
        perror("Erreur lors de la création du socket pour Pconfig");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse locale pour Pconfig

    memset(&adresse_locale, 0, sizeof(adresse_locale));
    adresse_locale.sin_family = AF_INET;
    adresse_locale.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    adresse_locale.sin_port = htons(port_local);

    // Lier le socket à l'adresse locale
    if (bind(socket_fd, (struct sockaddr*)&adresse_locale, sizeof(adresse_locale)) == -1) {
        perror("Erreur lors de la liaison du socket à l'adresse locale pour Pconfig");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    printf("Serveur démarré avec les paramètres suivants:\n");
    printf("Adresse IP : %s\n", inet_ntoa(adresse_locale.sin_addr));
    printf("Port : %d\n", port_local);

    // Mettre le socket en mode écoute
/*
    if (listen(socket_fd, 5) == -1) {
        perror("Erreur lors de la mise en écoute du socket");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    printf("[+]Listening...\n");
*/

    printf("Le serveur UDP attend des données...\n");

    while (1) {
        // Réception des données du client
        int bytes_received = recvfrom(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &client_len);
        if (bytes_received == -1) {
            perror("Erreur lors de la réception des données");
            break;
        }


       char*IP  =inet_ntoa( client_addr.sin_addr);
        int port_client=ntohs(client_addr.sin_port  );

        // Affichage des données reçues
        buffer[bytes_received] = '\0';
        printf("Données reçues du client : %s,%d\n",IP, port_client);

        // Réponse au client
        const char* response = "Message reçu par le serveur UDP.";
        sendto(socket_fd, response, strlen(response), 0, (struct sockaddr*)&client_addr, client_len);
    }

    close(socket_fd);
   // return 0;

}

// Fonction pour attendre que tous les processus Pi s'inscrivent et configurer l'anneau
int* inscription_clients(int n ) {
    printf("Pconfig est en attente des inscriptions des processus Pi...\n");

    int *clients = (int *) malloc(n * sizeof(int));
    int n_clients = 0;

    while (n_clients < n) {

        // Acceptation d'une connexion
        int clientfd = accept(socket_fd, NULL, NULL);

        if (clientfd < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("[+]New connection...\n");


        // Affiche les informations
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        getpeername(clientfd, (struct sockaddr *) &client_addr, &client_addr_len);

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        printf("Client connecté depuis : %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        // Ajouter le descripteur de socket du client au tableau
        // clients[n_clients] = clientfd;

        char buffer[256];
        recv(clientfd, buffer, sizeof(buffer), 0);
        printf("\nmessage recu :%s\n ", buffer);

        n_clients++;

      //  return clients;
    }
}

/*
void configuration(int n, int* clients){

    char port_msg[10];

    // Affiche les informations
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    char client_ip[INET_ADDRSTRLEN];
    for (int i = 0; i <n; ++i) {
        int voisin = (i + 1) % n;// Cas particulier : le dernier client reçoit le port du premier client
        getpeername(clients[voisin], (struct sockaddr *) &client_addr, &client_addr_len);

        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN); //pour IP besoin future
        snprintf(port_msg, sizeof(port_msg), "%d", ntohs(client_addr.sin_port));

        send(clients[i], port_msg, strlen(port_msg), 0);

    }

    for (int i = 0; i < n; ++n) {
        send(clients[i], "EXIT", strlen("EXIT"), 0);
        close(clients[i]);
    }

    close(socket_fd);
  //  exit(EXIT_SUCCESS);
   // close(clients[i]);

   }}

*/


    int main(int argc, char *argv[]) {
        close(socket_fd);
        int n = 0; //n nombre de processus/ client
        int port = -1; // Valeur par défaut si l'option -p n'est pas fournie
        int opt;

        // Utilisation de getopt pour gérer les arguments
        while ((opt = getopt(argc, argv, "n:p:")) != -1) {
            switch (opt) {
                case 'n':
                    if (sscanf(optarg, "%d", &n) != 1 || n <= 0) {
                        fprintf(stderr, "Erreur : l'argument de l'option -n doit être un entier positif.\n");
                        return 1;
                    }
                    break;
                case 'p':
                    if (sscanf(optarg, "%d", &port) != 1 || port <= 0) {
                        fprintf(stderr, "Erreur : l'argument de l'option -p doit être un entier positif.\n");
                        return 1;
                    }
                    break;
                default:
                    fprintf(stderr, "Utilisation : %s -n <nombre_total_de_processus> [-p <port>]\n", argv[0]);
                    return 1;
            }
        }

        // Vérifier si les valeurs des options ont été initialisées
        if (n == 0) {
            fprintf(stderr, "Erreur : l'option -n est obligatoire.\n"
                            "Utilisation : %s -n <nombre_total_de_processus> [-p <port>]\n", argv[0]);
            return 1;
        }

        if (port == -1) {
            printf("Aucun port spécifié. Utilisation du port par défaut : %d.\n", PORT_PAR_DEFAUT);
        }
        int port_utilise = (port == -1) ? PORT_PAR_DEFAUT : port;

        printf("\n *** En Attante de %d connection ***\n\n", n);

        // int* Ports= server(port_utilise,n);
        // Configuration et création du socket pour Pconfig
        configurer_socket(port_utilise);

        // Pconfig est prêt à recevoir les inscriptions et à configurer l'anneau
        int *clients = inscription_clients(n);
       // configuration(n, clients);

        // Pconfig n'a plus de rôle actif à jouer et peut se terminer
     //   printf("La configuration de l'anneau est terminée. Pconfig se termine.\n");
        close(socket_fd);

        return 0;
    }
