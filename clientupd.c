#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 5000
#define MAXLINE 1000

const uint16_t maxn = 5;

int main() {
    char buffer[MAXLINE];
    char *message = "Hello Server";
    int sockfd, n;
    struct sockaddr_in servaddr;

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Connect Failed");
        exit(EXIT_FAILURE);
    }

    send(sockfd, message, strlen(message), 0);

    for (int i = 0; i < maxn; i++) {
        memset(buffer, 0, sizeof(buffer));
        n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (n < 0) {
            perror("Receive Failed");
            exit(EXIT_FAILURE);
        }
        buffer[n] = '\0';
        puts(buffer);

        char ans;
        scanf(" %c", &ans);  
        printf("\n");

        send(sockfd, &ans, 1, 0);

        memset(buffer, 0, sizeof(buffer));
        n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (n < 0) {
            perror("Receive Failed");
            exit(EXIT_FAILURE);
        }
        buffer[n] = '\0';
        puts(buffer);
        printf("\n");
    }

    memset(buffer, 0, sizeof(buffer));
    n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (n < 0) {
        perror("Receive Failed");
        exit(EXIT_FAILURE);
    }
    buffer[n] = '\0';
    puts(buffer);

    close(sockfd);
    return 0;
}
