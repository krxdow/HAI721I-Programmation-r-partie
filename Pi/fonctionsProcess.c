
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <limits.h>
#include "fonctionsProcess.h"

#define PORT_PAR_DEFAUT 4445

// Définition des variables globales
 int  id_local;              // Le numéro de ce processus donner par Pconfig
 int socket_fd;
 int sockfd_serv_loc;
 // buffer de recpection InfoProcessus voisin
 struct sockaddr_in servaddr= {0} ,voisin_addr;
 socklen_t servaddr_len = sizeof(servaddr);
char buffer[sizeof(struct sockaddr_in)]={0};

InfoProcessus infoProcessus;

void send_socket_addr(struct sockaddr_in server_local) {
    char buffer_send[sizeof(struct sockaddr_in)];
    memset(buffer_send, 0, sizeof(struct sockaddr_in));

    // Serialisation de la structure sockaddr_in
    memcpy(buffer_send, &server_local, sizeof(server_local));

    // Envoi de sockaddr_in server_local au serveur principal
    ssize_t bytes_sent = sendto(socket_fd, buffer_send, sizeof(buffer_send), 0,
                                (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (bytes_sent == -1) {
        perror("Erreur lors de l'envoi des données");
        close(socket_fd);
        exit(EXIT_FAILURE);
    } else {
        printf("Bytes sent: %zd\n", bytes_sent);
    }

    printf("Structure describing an Internet socket address sent\n");
}



char* recv_socket_addr(){


    // Réception de la réponse du serveur

    memset(&infoProcessus, 0, (sizeof(InfoProcessus)));

   // char buffrecv[sizeof( infoProcessus)];
    char* buffrecv = (char*)malloc((sizeof(InfoProcessus)));
    memset(buffrecv, 0, (sizeof(&buffrecv)));

   // Assurez-vous de vider le tampon avant la réception
    ssize_t  bytes_received = recvfrom(socket_fd, buffrecv, sizeof(infoProcessus), 0, NULL, NULL);
    if (bytes_received == -1) {
        perror("Erreur lors de la réception de la réponse du serveur");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }else {
        printf("bytes_received  %zd bytes \n", bytes_received);
    }


    printf("reception structure voisin ok\n");
    return (char *) buffrecv;
}

void configuration(char * buff){


    // Analysez les données reçues pour obtenir les informations du voisin
    memset(&voisin_addr,0,sizeof(voisin_addr));
    memset(&infoProcessus, 0, sizeof(infoProcessus));

    memcpy(&infoProcessus, buff, sizeof(InfoProcessus ));
    voisin_addr = infoProcessus.client_addr;
   // voisin_addr.sin_family=AF_INET;

    // Utilisez les données désérialisées
    printf("\n\n");
    id_local = infoProcessus.current_id;
    printf("Mon ID : %d\n", id_local);
    printf("Info du voisin\n");
    printf("Numéro id du voisin : %d\n", infoProcessus.neighbor_id);
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, (&voisin_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("Adresse IP : %s\n", client_ip);
    printf("Port : %d\n", ntohs(infoProcessus.client_addr.sin_port));
    // Mise à jour de l'ID ici




    printf("Configuration voisin et mise à jours ID ok\n");

}



struct sockaddr_in serveur_local(){
    struct sockaddr_in server_addr;


    // Créez le socket
    sockfd_serv_loc = socket(AF_INET, SOCK_STREAM , 0);
    if (sockfd_serv_loc == -1) {
        perror("Erreur lors de la création du socket");
        exit(1);
    }

    // Initialisez la structure du serveur
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Laissez le système choisir l'adresse IP
    server_addr.sin_port = 0; // Laissez le système attribuer automatiquement le port

    // Liez le socket au port et à l'adresse IP
    if (bind(sockfd_serv_loc, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur lors de la liaison du socket");
        close(sockfd_serv_loc);
        exit(EXIT_FAILURE);
    }


    // Récupérer le numéro de port attribué par le système
    socklen_t addr_len = sizeof(server_addr);
    if (getsockname(sockfd_serv_loc, (struct sockaddr*)&server_addr, &addr_len) == -1) {
        perror("Erreur lors de la récupération du numéro de port");
        close(sockfd_serv_loc);
        exit(EXIT_FAILURE);
    }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(server_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

    printf("Le serveur local écoute sur -> %s:%d\n",client_ip ,ntohs(server_addr.sin_port));



    return server_addr;

}



// Fonction pour se connecter au serveur principal
int connection(int port_utilise) {
    printf("port udp %d\n", port_utilise);

    // Création du socket
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // configuration de l'adresse du serveur principal
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(port_utilise);


    // Obtenir des informations sur le client
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    // Utilisez getsockname pour obtenir les informations du socket local
    if (getsockname(socket_fd, (struct sockaddr *) &client_addr, &client_addr_len) == -1) {
        perror("Erreur lors de l'obtention des informations du socket local");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("\nConnecté au serveur principal\nPort local du client : %d\n", ntohs(servaddr.sin_port));

    return socket_fd ;

}
void connect_neighbor() {

    int sockfd_to_neighbor = socket(AF_INET, SOCK_STREAM , 0);
    if (sockfd_to_neighbor == -1) {
        perror("Erreur lors de la création de la socket");
        exit(EXIT_FAILURE);
    }
    int connect_result;


    if(id_local==0){
        printf("premier process\n");
    }
    while (1) {
        connect_result = connect(sockfd_to_neighbor, (struct sockaddr*)&voisin_addr, sizeof(voisin_addr));
        if (connect_result == 0) {
            // Connexion réussie
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(voisin_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

            printf("Connexion au voisin réussie\n");
            printf("Sur le port %u et IP : %s\n", ntohs(voisin_addr.sin_port), client_ip);
            break; // Sortez de la boucle une fois que la connexion est établie
        } else if (connect_result == -1 && (errno == EINPROGRESS || errno == EALREADY)) {
            // Connexion en cours, continuez à essayer
            printf("La connexion est en cours...\n");
            sleep(1); // Attendez un moment avant de réessayer
        } else {
            // Une erreur s'est produite lors de la connexion
            perror("Erreur lors de la connexion");
            fprintf(stderr, "Code d'erreur : %d\n", errno);

            // Vous pouvez ajouter un code pour gérer les erreurs spécifiques ici

            //  close(sockfd_to_neighbor);
            //     exit(EXIT_FAILURE);
        }
    }

}


int accepte_listen_connection() {
    if (listen(sockfd_serv_loc, 10) == -1) {
        perror("Erreur lors de la liaison du socket");
        close(sockfd_serv_loc);
        exit(EXIT_FAILURE);
    } else {
        printf("\nServeur local en attente de connexion\n");
    }

    struct sockaddr_in new_addr = {0};
    socklen_t addr_len = sizeof(new_addr);
    int new_socket_fd;

    while (1) {
        new_socket_fd = accept(sockfd_serv_loc, (struct sockaddr *)&new_addr, &addr_len);

        if (new_socket_fd == -1) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                // Aucune connexion en attente, continuez à attendre
                continue;
            } else {
                // Une erreur s'est produite lors de l'acceptation de la connexion
                perror("Erreur lors de l'acceptation de la connexion");
                close(sockfd_serv_loc);
                exit(EXIT_FAILURE);
            }
        } else {
            // Connexion acceptée
            printf("\nConnection voisin ok\n");

            // Vous pouvez continuer à utiliser new_socket_fd pour communiquer avec le client

            // Fermez la socket lorsque vous avez terminé avec elle
            //     close(new_socket_fd);
            break; // Sortez de la boucle une fois que la connexion est acceptée
        }
    }
    return new_socket_fd ;
}


// Fonction pour gérer les arguments et obtenir le numéro de port
int parse_arguments(int argc, char *argv[]) {
    int port = -1; // Valeur par défaut si l'option -p n'est pas fournie
    int opt;

    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p': {
                char *endptr;
                errno = 0; // Réinitialise errno avant l'appel à strtol
                long int port_long = strtol(optarg, &endptr, 10);

                if ((errno == ERANGE && (port_long == LONG_MAX || port_long == LONG_MIN)) || (errno != 0 && port_long == 0)) {
                    fprintf(stderr, "Erreur : conversion de l'argument de l'option -p en entier échouée (dépassement de capacité ou argument non numérique).\n");
                    exit(1);
                }

                if (endptr == optarg || *endptr != '\0') {
                    fprintf(stderr, "Erreur : l'argument de l'option -p n'est pas un entier valide.\n");
                    exit(1);
                }

                port = (int)port_long;

                if (port <= 0) {
                    fprintf(stderr, "Erreur : l'argument de l'option -p doit être un entier positif.\n");
                    exit(1);
                }
                break;
            }
            default:
                fprintf(stderr, "Utilisation : %s [-p <port>]\n", argv[0]);
                exit(1);
        }
    }

    return port;
}


int send_id(int fd){
    char buff[1024];

    // Copier l'entier dans le tampon
    memcpy(buffer, &id_local, sizeof(id_local));

    // Taille des données à envoyer
    size_t data_size = sizeof(id_local);

    // Utilisation de ssize_t pour gérer les valeurs de retour de send
    ssize_t bytes_sent = send(fd, buffer, data_size, 0);

    if (bytes_sent == -1) {
        perror("Erreur lors de l'envoi");
        exit(EXIT_FAILURE);
    }

    printf("Envoyé %zd octets de données.\n", bytes_sent);

    return 0;
}

int recieve_id(int fd){
    int received_id; // Variable pour stocker l'entier reçu
    char buff[1024];

    // Taille des données à recevoir
    size_t data_size = sizeof(received_id);

    // Utilisation de ssize_t pour gérer les valeurs de retour de recv
    ssize_t bytes_received = recv(fd, buffer, data_size, 0);

    if (bytes_received == -1) {
        perror("Erreur lors de la réception");
        exit(EXIT_FAILURE);
    } else if (bytes_received == 0) {
        // La connexion a été fermée par le client
        printf("La connexion a été fermée par le client.\n");
        return -1; // Ou une autre valeur pour indiquer une erreur de réception
    }

    if (bytes_received != data_size) {
        fprintf(stderr, "Erreur : nombre d'octets reçus incorrect.\n");
        exit(EXIT_FAILURE);
    }

    // Copier les données reçues dans l'entier
    memcpy(&received_id, buffer, data_size);

    printf("Reçu un entier : %d\n", received_id);

    return received_id;
}

int size_calculator(int fd){
    int size = 0;
    while (recieve_id(fd)!=id_local) {
        size++;
    }
    return size;
}


