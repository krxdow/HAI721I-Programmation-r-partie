//
// Created by ludovic.fanus@etu.umontpellier.fr on 22/09/23.
//

#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>

#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include "fonctionsProcess.h"





int main(int argc, char *argv[]) {




    close(socket_fd);//liber le port pour relancer




    int port_utilise = parse_arguments(argc, argv);
    if (port_utilise == -1) {
       port_utilise=PORT_PAR_DEFAUT;
       printf("Aucun port spécifié. Utilisation du port par défaut : %d pour se connecter au serveur.\n", PORT_PAR_DEFAUT);
    }

    connection(port_utilise);
    struct sockaddr_in addr;
     memset(&addr,0, sizeof(addr));
     addr  = serveur_local();
printf("address envoye\n");
     printf("address : %s port : %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

    char* buffrecv = (char*)malloc((sizeof(InfoProcessus)));
    memset(buffrecv, 0, sizeof(InfoProcessus));

    send_socket_addr(addr);
    buffrecv= recv_socket_addr();
    configuration(buffrecv);

    printf("\n");

    int new_socket_fd;
    int size=0;

    if (id_local == 0) {
       // connect_neighbor();
        //new_socket_fd=accepte_listen_connection();
      //  send_id(new_socket_fd);
       // size=size_calculator(new_socket_fd);
        //recieve_id(new_socket_fd);
    }else{
        //new_socket_fd=accepte_listen_connection();
        //connect_neighbor();
        //recieve_id(new_socket_fd);
        //send_id(new_socket_fd);
        //size=size_calculator(new_socket_fd);
    }

    printf("taille de l'anneau:%d\n",size);

    return 0;
}
