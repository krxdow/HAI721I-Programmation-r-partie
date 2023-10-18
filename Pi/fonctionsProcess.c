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

int id_local;
int socket_fd;
int sockfd_serv_loc;
struct sockaddr_in servaddr = {0}, voisin_addr = {0};
socklen_t servaddr_len = sizeof(servaddr);
char buffer[sizeof(struct sockaddr_in)] = {0};

InfoProcessus infoProcessus;

// Function to send a sockaddr_in structure to the server
void send_socket_addr(struct sockaddr_in server_local) {
    char buffer_send[sizeof(struct sockaddr_in)];
    memset(buffer_send, 0, sizeof(struct sockaddr_in));

    // Serialize the sockaddr_in structure
    memcpy(buffer_send, &server_local, sizeof(server_local));

    // Send sockaddr_in server_local to the main server
    ssize_t bytes_sent = sendto(socket_fd, buffer_send, sizeof(buffer_send), 0,
                                (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (bytes_sent == -1) {
        perror("Error sending data");
        close(socket_fd);
        exit(EXIT_FAILURE);
    } else {
        printf("Bytes sent: %zd\n", bytes_sent);
    }

    printf("Structure describing an Internet socket address sent\n");
}

// Function to receive sockaddr_in structure from the server
char *recv_socket_addr() {
    // Receive the server's response
    memset(&infoProcessus, 0, sizeof(InfoProcessus));
    char *buffrecv = (char *)malloc(sizeof(InfoProcessus));
    // Make sure to clear the buffer before receiving
    memset(buffrecv, 0, sizeof(InfoProcessus));

    ssize_t bytes_received = recvfrom(socket_fd, buffrecv, sizeof(infoProcessus), 0, NULL, NULL);
    if (bytes_received == -1) {
        perror("Error receiving server response");
        close(socket_fd);
        exit(EXIT_FAILURE);
    } else {
        printf("Received %zd bytes\n", bytes_received);
    }

    printf("Received neighbor structure successfully\n");
    return (char *)buffrecv;
}

// Function to process configuration data
void configuration(char *buff) {
    // Parse the received data to get neighbor information
    memset(&voisin_addr, 0, sizeof(voisin_addr));
    memset(&infoProcessus, 0, sizeof(infoProcessus));

    memcpy(&infoProcessus, buff, sizeof(InfoProcessus));
    voisin_addr = infoProcessus.client_addr;

    printf("\n\n");
    id_local = infoProcessus.current_id;

    printf("My ID: %d\n", id_local);
    printf("Neighbor Info\n");
    printf("Neighbor ID: %d\n", infoProcessus.neighbor_id);
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, (&voisin_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("IP Address: %s\n", client_ip);
    printf("Port: %d\n", ntohs(infoProcessus.client_addr.sin_port));

    printf("Neighbor configuration and ID update completed\n");
}

// Function to configure the local server
struct sockaddr_in serveur_local() {
    struct sockaddr_in server_addr;

    sockfd_serv_loc = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_serv_loc == -1) {
        perror("Error creating socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = 0;
    int server_addr_len = sizeof(server_addr);

    if (bind(sockfd_serv_loc, (struct sockaddr *)&server_addr, server_addr_len)) {
        perror("Error binding socket");
        close(sockfd_serv_loc);
        exit(EXIT_FAILURE);
    }

    socklen_t addr_len = sizeof(server_addr);
    if (getsockname(sockfd_serv_loc, (struct sockaddr *)&server_addr, &addr_len) == -1) {
        perror("Error retrieving port number");
        close(sockfd_serv_loc);
        exit(EXIT_FAILURE);
    }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(server_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

    printf("Local server is listening on -> %s:%d\n", client_ip, ntohs(server_addr.sin_port));

    return server_addr;
}

// Function to connect to the main server
int connection(int port_utilise) {
    printf("UDP port: %d\n", port_utilise);

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(port_utilise);

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    if (getsockname(socket_fd, (struct sockaddr *)&client_addr, &client_addr_len) == -1) {
        perror("Error getting socket information");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("\nConnected to the main server\nLocal client port: %d\n", ntohs(servaddr.sin_port));

    return socket_fd;
}

int connect_neighbor() {
    int sockfd_to_neighbor = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_to_neighbor == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    int connect_result;

    if (id_local == 0) {
        printf("First process\n");
    }
    while (1) {
        connect_result = connect(sockfd_to_neighbor, (struct sockaddr *)&voisin_addr, sizeof(voisin_addr));
        if (connect_result == 0) {
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(voisin_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

            printf("Connection to neighbor succeeded\n");
            printf("On port %u and IP: %s\n", ntohs(voisin_addr.sin_port), client_ip);
            return sockfd_to_neighbor;
            break;
        } else if (connect_result == -1 && (errno == EINPROGRESS || errno == EALREADY)) {
            printf("Connection is in progress...\n");
            sleep(1);
        } else {
            perror("Error during connection");
            fprintf(stderr, "Error code: %d\n", errno);
        }
    }
}

int accepte_listen_connection() {
    if (listen(sockfd_serv_loc, 10) == -1) {
        perror("Error binding socket");
        close(sockfd_serv_loc);
        exit(EXIT_FAILURE);
    } else {
        printf("\nLocal server waiting for connections\n");
    }

    struct sockaddr_in new_addr;
    memset(&new_addr, 0, sizeof(new_addr));
    socklen_t addr_len = sizeof(new_addr);
    int new_socket_fd;

    while (1) {
        new_socket_fd = accept(sockfd_serv_loc, (struct sockaddr *)&new_addr, (socklen_t *)&addr_len);

        if (new_socket_fd == -1) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                continue;
            } else {
                perror("Error accepting connection");
            }
        } else {
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(new_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

            printf("Connection accepted\n");
            printf("On port %u and IP: %s\n", ntohs(new_addr.sin_port), client_ip);
            return new_socket_fd;
        }
        break;
    }

    return new_socket_fd;
}

int parse_arguments(int argc, char *argv[]) {
    int port = -1;
    int opt;

    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p': {
                char *endptr;
                errno = 0;
                long int port_long = strtol(optarg, &endptr, 10);

                if ((errno == ERANGE && (port_long == LONG_MAX || port_long == LONG_MIN)) || (errno != 0 && port_long == 0)) {
                    fprintf(stderr, "Error: conversion of the -p option argument to an integer failed (capacity overflow or non-numeric argument).\n");
                    exit(1);
                }

                if (endptr == optarg || *endptr != '\0') {
                    fprintf(stderr, "Error: -p option argument is not a valid integer.\n");
                    exit(1);
                }

                port = (int)port_long;

                if (port <= 0) {
                    fprintf(stderr, "Error: -p option argument must be a positive integer.\n");
                    exit(1);
                }
                break;
            }
            default:
                fprintf(stderr, "Usage: %s [-p <port>]\n", argv[0]);
                exit(1);
        }
    }

    return port;
}

int send_id(int fd, Ring_size ring_size) {
    char buff[1024];
    memset(buff, 0, sizeof(buff));

    memcpy(buff, &ring_size, sizeof(ring_size));

    ssize_t bytes_sent = send(fd, buff, sizeof(buff), 0);
    if (bytes_sent == -1) {
        perror("Error sending");
        exit(EXIT_FAILURE);
    }
    memcpy(&ring_size, buff, sizeof(ring_size));
    printf("Sent ring_size ID %d\n", ring_size.id);

    return 0;
}

Ring_size recieve_id(int fd) {
    char buff[1024];
    Ring_size ring_size_recv;
    memset(&ring_size_recv, 0, sizeof(ring_size_recv));
    ssize_t bytes_received = recv(fd, buff, sizeof(buff), 0);

    if (bytes_received == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            printf("Connection is waiting for data\n");
        } else {
            perror("Error receiving");
            exit(EXIT_FAILURE);
        }
    } else if (bytes_received == 0) {
        printf("Connection was closed by the client.\n");
    } else {
        memcpy(&ring_size_recv, buff, sizeof(ring_size_recv));
        printf("Received ID: %d, previous size: %d\n", ring_size_recv.id, ring_size_recv.size);
    }

    return ring_size_recv;
}
