//
// Created by dow on 07/10/23.
//

#ifndef RINGSTRUCT_FONCTIONSPROCESS_H
#define RINGSTRUCT_FONCTIONSPROCESS_H

#include <netinet/in.h>

#define PORT_PAR_DEFAUT 4445

typedef struct {
    int current_id;
    int neighbor_id;
    struct sockaddr_in client_addr;
} InfoProcessus;


extern int id_local;
extern int socket_fd;
extern int sockfd_serv_loc;
extern char buffer[sizeof(struct sockaddr_in)]; // buffer de recpection InfoProcessus voisin
extern struct sockaddr_in servaddr ,voisin_addr;
extern socklen_t servaddr_len;


int parse_arguments(int,char *[]);
void send_socket_addr(struct sockaddr_in );
char* recv_socket_addr();
void configuration(char *);
struct sockaddr_in serveur_local();
int connection(int);
void connect_neighbor();
int accepte_listen_connection();
void func(int);
int send_id(int);
int recieve_id(int);
int size_calculator(int);



#endif //RINGSTRUCT_FONCTIONSPROCESS_H