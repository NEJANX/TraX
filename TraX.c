#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

void *receive_messages(void *arg);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <server_ip>\n", argv[0]);
        return -1;
    }

    char *server_ip = argv[1];
    int client_socket;
    struct sockaddr_in server_addr;
    pthread_t tid;
    char buffer[BUFFER_SIZE];

    printf("Enter your username: ");
    char username[32];
    fgets(username, 32, stdin);
    username[strcspn(username, "\n")] = 0;

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(57644);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        return -1;
    }

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    pthread_create(&tid, NULL, receive_messages, (void*)&client_socket);

    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        char message[BUFFER_SIZE + 32];
        sprintf(message, "%s: %s", username, buffer);

        send(client_socket, message, strlen(message), 0);
    }

    close(client_socket);
    return 0;
}

void *receive_messages(void *arg) {
    int client_socket = *(int*)arg;
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[bytes_received] = '\0';
        printf("%s\n", buffer);
    }

    return NULL;
}
