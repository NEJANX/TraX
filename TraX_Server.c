#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 57644
#define BUFFER_SIZE 1024

void *handle_client(void *arg);
void broadcast_message(char *message, int exclude_fd);

int client_sockets[100];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    pthread_t tid;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Chat server started on port %d\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        pthread_mutex_lock(&clients_mutex);
        client_sockets[client_count++] = new_socket;
        pthread_mutex_unlock(&clients_mutex);

        pthread_create(&tid, NULL, handle_client, (void*)&new_socket);
    }

    return 0;
}

void *handle_client(void *arg) {
    int client_socket = *(int*)arg;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE + 32];
    int bytes_received;

    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[bytes_received] = '\0';
        sprintf(message, "%s", buffer);
        broadcast_message(message, client_socket);
    }

    close(client_socket);
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (client_sockets[i] == client_socket) {
            for (int j = i; j < client_count - 1; j++) {
                client_sockets[j] = client_sockets[j + 1];
            }
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    pthread_exit(NULL);
}

void broadcast_message(char *message, int exclude_fd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (client_sockets[i] != exclude_fd) {
            send(client_sockets[i], message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}
