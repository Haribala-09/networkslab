// client.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define SHIFT 5

void decryptMessage(char *encrypted, int shift, char *decrypted) {
    int i;
    for (i = 0; encrypted[i] != '\0'; i++) {
        if (encrypted[i] >= 'a' && encrypted[i] <= 'z')
            decrypted[i] = ((encrypted[i] - 'a' - shift + 26) % 26) + 'a';
        else if (encrypted[i] >= 'A' && encrypted[i] <= 'Z')
            decrypted[i] = ((encrypted[i] - 'A' - shift + 26) % 26) + 'A';
        else
            decrypted[i] = encrypted[i];
    }
    decrypted[i] = '\0';
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char decryptedMessage[1024] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    read(sock, buffer, 1024);
    decryptMessage(buffer, SHIFT, decryptedMessage);
    printf("Received encrypted message: \"%s\"\n", buffer);
    printf("Decrypted to original message: \"%s\"\n", decryptedMessage);

    close(sock);
    return 0;
}
