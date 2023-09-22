//
// Created by ludovic.fanus@etu.umontpellier.fr on 22/09/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
// Fonction pour lancer la configuration de l'anneau
void configurer_anneau(int n) {
    printf("La configuration de l'anneau est terminée. P0 se termine.\n");
}
#define PORT 4444
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage : %s <numéro de port>\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]); // Convertit l'argument en entier

    int sockfd;
    struct sockaddr_in serverAddr;

    int newSocket;
    struct sockaddr_in newAddr;

        socklen_t addr_size;
        char buffer[1024];

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        printf("[+]Server Socket Created Sucessfully.\n");
        memset(&serverAddr, '\0', sizeof(serverAddr));

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(PORT);
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        bind(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
        printf("[+]Bind to Port number %d.\n", PORT);

        listen(sockfd, 5);
        printf("[+]Listening...\n");

        newSocket = accept(sockfd, (struct sockaddr *) &newAddr, &addr_size);

        strcpy(buffer, "Hello");
        send(newSocket, buffer, strlen(buffer), 0);
        printf("[+]Closing the connection.\n");


        // Lancer la configuration de l'anneau avec le nombre total de processus (n)
        configurer_anneau(n);

        return 0;
    }
