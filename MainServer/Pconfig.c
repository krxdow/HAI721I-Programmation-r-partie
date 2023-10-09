#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT_DEFAULT 4445

// Structure definition for InfoProcessus
typedef struct {
    int current_id;
    int neighbor_id;
    struct sockaddr_in  client_addr;
} InfoProcessus;

int socket_fd;

// Function to configure and create the socket
void configure_socket(int local_port, int n) {
    struct sockaddr_in local_address;
    char buffer_recv_port[1024];

    // Create a UDP socket
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1) {
        perror("Error creating socket for Pconfig");
        exit(EXIT_FAILURE);
    }

    // Configure local address for Pconfig
    memset(&local_address, 0, sizeof(local_address));
    local_address.sin_family = AF_INET;
    local_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    local_address.sin_port = htons(local_port);

    // Bind the socket to the local address
    if (bind(socket_fd, (struct sockaddr*)&local_address, sizeof(local_address)) == -1) {
        perror("Error binding socket to local address for Pconfig");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server started with the following settings:\n");
    printf("IP Address: %s\n", inet_ntoa(local_address.sin_addr));
    printf("Port: %d\n", local_port);

    printf("The UDP server is waiting for data...\n");

    InfoProcessus infoProcessus[n];
    memset(&infoProcessus, 0, sizeof(infoProcessus));


    struct  sockaddr_in client_ips[n];
    socklen_t ClientLen[n]; // Create an array to hold the lengths
    memset(client_ips, 0, sizeof(client_ips));


    struct sockaddr_in client_server_local = {0};

    int i = 0;
    while (i < n) {
        socklen_t len = sizeof(client_ips[i].sin_addr);
        ClientLen[i] = sizeof(client_ips[i]);


        size_t bytes_received = recvfrom(socket_fd,
                                         buffer_recv_port,
                                         sizeof(buffer_recv_port), 0,
                                         (struct sockaddr*)&client_ips[i].sin_addr,
                                          ClientLen);
        if (bytes_received == -1) {
            perror("Error receiving data");
            break;
        } else {

            memcpy(&infoProcessus[i].client_addr, buffer_recv_port, sizeof(struct sockaddr_in));

            printf("Reçuy de %zd bytes \n", bytes_received);
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(infoProcessus[i].client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

            printf("Connection Info for process %s:%d:\n", client_ip, ntohs(infoProcessus[i].client_addr.sin_port));
            infoProcessus[i].current_id = i;
            i++;
        }

    }

    char buffer_send[sizeof(InfoProcessus)];
    int voisin =0;


    struct sockaddr_in first_infoProcess  = infoProcessus[0].client_addr;

    for (int j = 0; j < n; ++j) {
        memset(buffer_send, 0, sizeof(buffer_send));

        voisin= (j + 1) % n;
        infoProcessus[j].neighbor_id = voisin;
        memset(&infoProcessus[j].client_addr, 0,sizeof(infoProcessus[j].client_addr));

        if (voisin==0){
            infoProcessus[n-1].client_addr = first_infoProcess;
        } else{
            infoProcessus[j].client_addr = infoProcessus[voisin].client_addr;
        }


      //  infoProcessus[j].client_addr.sin_port = infoProcessus[voisin].client_addr.sin_port;
      //memcpy(&infoProcessus[j].client_addr, &infoProcessus[voisin].client_addr, sizeof(infoProcessus[voisin].client_addr) );


        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(infoProcessus[j].client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        printf("Verification of the structure sent: process %d, neighbor %d, IP: %s, Port: %d\n",
               infoProcessus[j].current_id, infoProcessus[j].neighbor_id, client_ip, ntohs(infoProcessus[j].client_addr.sin_port));

        memcpy(buffer_send, &infoProcessus[j], sizeof(buffer_send));

        ssize_t bytes_sent = sendto(socket_fd, buffer_send,
                                    sizeof(buffer_send), 0,
                                    (struct sockaddr*)&client_ips[j].sin_addr, ClientLen[j]);
        if (bytes_sent == -1) {
            perror("Error sending data to neighbor");
            break;
        } else {
            printf("Sent %zd bytes to process %d\n", bytes_sent, j);
        }
    }
}


int main(int argc, char *argv[]) {
    close(socket_fd);
    int n = 0;
    int port = -1;
    int opt;

    while ((opt = getopt(argc, argv, "n:p:")) != -1) {
        switch (opt) {
            case 'n':
                if (sscanf(optarg, "%d", &n) != 1 || n <= 0) {
                    fprintf(stderr, "Error: The argument for option -n must be a positive integer.\n");
                    return 1;
                }
                break;
            case 'p':
                if (sscanf(optarg, "%d", &port) != 1 || port <= 0) {
                    fprintf(stderr, "Error: The argument for option -p must be a positive integer.\n");
                    return 1;
                }
                break;
            default:
                fprintf(stderr, "Usage: %s -n <total_number_of_processes> [-p <port>]\n", argv[0]);
                return 1;
        }
    }

    if (n == 0) {
        fprintf(stderr, "Error: The -n option is mandatory.\n"
                        "Usage: %s -n <total_number_of_processes> [-p <port>]\n", argv[0]);
        return 1;
    }

    if (port == -1) {
        printf("No port specified. Using the default port: %d.\n", PORT_DEFAULT);
    }
    int used_port = (port == -1) ? PORT_DEFAULT : port;

    configure_socket(used_port, n);

    return 0;
}
