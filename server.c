#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define BROADCAST_ADDRESS "127.0.0.255" 
#define PORT 5000

int clients[MAX_CLIENTS];
int client_count = 0;

void broadcast(char *message, int sender_fd) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        int client_socket = clients[i];
        if (client_socket > 0 && client_socket != sender_fd) {
            send(client_socket, message, strlen(message), 0);
        }
    }
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    char buffer[BUFFER_SIZE];

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 5000\n");

    // Accept connections from clients
    while (1) {
        socklen_t client_address_len = sizeof(client_address);
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);

        if (client_socket < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Add client to the list
        clients[client_count++] = client_socket;
        printf("Client connected, total clients: %d\n", client_count);

        // Receive and broadcast messages
        while (1) {
            memset(buffer, 0, sizeof(buffer));
            int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

            if (bytes_received <= 0) {
                // Connection closed or error
                printf("Client disconnected\n");
                close(client_socket);
                break;
            }

            // Broadcast the message to all clients
            broadcast(buffer, client_socket);
        }
    }

    // Close the server socket
    close(server_socket);

    return 0;
}