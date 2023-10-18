#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "fonctionsProcess.h"
#include <signal.h>
#include <ctype.h>
#include <sys/time.h>
typedef struct {
    int id;
    int id_neighbor;
    char message[1024];
} Message;


// Fonction pour échanger l'ID avec le voisin et calculer la taille de l'anneau
void exchangeIDAndCalculateRingSize(const int *new_socket_fd_connect, const int *new_socket_fd_accept,
                                    Ring_size *ring_size_send, Ring_size *ring_size_recv) {
    send_id(*new_socket_fd_connect, *ring_size_send);
    while (ring_size_recv->id != id_local) {
        *ring_size_recv = recieve_id(*new_socket_fd_accept);
        if (ring_size_recv->id == id_local) {
            ring_size_recv->size++;
            ring_size_send->size = ring_size_recv->size;
            break;
        }
        ring_size_recv->size++;
        send_id(*new_socket_fd_connect, *ring_size_recv);
    }
}

volatile int inputnumber = -1;

void handle_alarm(int signum) {
    // Cette fonction sera appelée lorsque l'alarme expire
    inputnumber = -1; // Marquez la saisie comme invalide
}




struct itimerval timmer(int seconds){
    // Activation de l'alarme pendant 10 secondes
    struct itimerval timer;
    timer.it_value.tv_sec = seconds; // Temps initial
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0; // Intervalle entre les expirations (0 pour une seule expiration)
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
    return timer;
}

void unactivate_alarm( struct itimerval *timer){
    // Désactivation de l'alarme
    timer->it_value.tv_sec = 0;
    timer->it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, timer, NULL);
}


int main(int argc, char *argv[]) {


    close(socket_fd);



    struct sigaction sa;
    sa.sa_handler = handle_alarm;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGALRM, &sa, NULL);


    int port_utilise = parse_arguments(argc, argv);
    if (port_utilise == -1) {
        port_utilise = PORT_PAR_DEFAUT;
        printf("Aucun port spécifié. Utilisation du port par défaut : %d pour se connecter au serveur.\n",
               PORT_PAR_DEFAUT);
    }
    connection(port_utilise);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr = serveur_local();
    printf("Adresse envoyée\n");
    printf("Adresse : %s Port : %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    char *buffrecv = (char *) malloc((sizeof(InfoProcessus)));
    memset(buffrecv, 0, sizeof(InfoProcessus));
    send_socket_addr(addr);
    buffrecv = recv_socket_addr();
    configuration(buffrecv);
    printf("\n");

    int new_socket_fd_connect = 0;
    int new_socket_fd_accept = 0;
    Ring_size ring_size_recv;
    ring_size_recv.id = -1;
    Ring_size ring_size_send;
    ring_size_send.id = id_local;
    ring_size_send.size = 0;

    if (id_local == 0) {
        new_socket_fd_connect = connect_neighbor();
        new_socket_fd_accept = accepte_listen_connection();
    } else {
        new_socket_fd_accept = accepte_listen_connection();
        new_socket_fd_connect = connect_neighbor();
    }
    exchangeIDAndCalculateRingSize(&new_socket_fd_connect, &new_socket_fd_accept, &ring_size_send, &ring_size_recv);
    printf("Ring size : %d\n", ring_size_send.size);


    return 0;
}