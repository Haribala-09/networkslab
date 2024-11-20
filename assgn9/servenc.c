// server.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 2
#define SHIFT 5

void encryptMessage(char *message, int shift, char *encrypted)
{
    int i;
    for (i = 0; message[i] != '\0'; i++)
    {
        if (message[i] >= 'a' && message[i] <= 'z')
            encrypted[i] = ((message[i] - 'a' + shift) % 26) + 'a';
        else if (message[i] >= 'A' && message[i] <= 'Z')
            encrypted[i] = ((message[i] - 'A' + shift) % 26) + 'A';
        else
            encrypted[i] = message[i];
    }
    encrypted[i] = '\0';
    puts(encrypted);
}

int main()
{
    int server_fd, new_socket, clients[MAX_CLIENTS];
    struct sockaddr_in address;
    int addrlen = sizeof(address), client_count = 0;
    char *message = "attack p30";
    char encryptedMessage[1024] = {0};

    encryptMessage(message, SHIFT, encryptedMessage);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (client_count < MAX_CLIENTS)
    {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0)
        {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        clients[client_count++] = new_socket;
        printf("Client %d connected\n", client_count);
    }

    for (int i = 0; i < client_count; i++)
    {
        send(clients[i], encryptedMessage, strlen(encryptedMessage), 0);
        close(clients[i]);
    }

    close(server_fd);
    return 0;
}
