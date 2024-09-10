#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 5000
#define SZ 300

void display_option(int sockfd)
{
    char receive_buffer[SZ];
    ssize_t len = read(sockfd, receive_buffer, sizeof(receive_buffer) - 1);
    if (len >= 0)
    {
        receive_buffer[len] = '\0';
        printf("%s", receive_buffer);
    }
    else
    {
        perror("Read error");
    }
}

void book_option(int sockfd)
{
    char receive_buffer[SZ];
    char send_buffer[SZ];
    ssize_t len;

    printf("hello");
    fflush(stdout);

    printf("Enter the movie tickets followed by number of seats: ");
    fflush(stdout);

    if (fgets(send_buffer, sizeof(send_buffer), stdin) == NULL)
    {
        perror("fgets error");
        return;
    }

    size_t input_len = strlen(send_buffer);
    if (input_len > 0 && send_buffer[input_len - 1] == '\n')
    {
        send_buffer[input_len - 1] = '\0';
    }

    printf("Sending: %s\n", send_buffer);

    ssize_t written = write(sockfd, send_buffer, strlen(send_buffer));
    if (written < 0)
    {
        perror("Write error");
        return;
    }

    memset(receive_buffer, 0, sizeof(receive_buffer));
    len = read(sockfd, receive_buffer, sizeof(receive_buffer) - 1);
    if (len < 0)
    {
        perror("Read error");
        return;
    }
    receive_buffer[len] = '\0';
    printf("Received: %s\n", receive_buffer);
    fflush(stdout);
}

void cancel_option(int sockfd)
{
    char receive_buffer[SZ];
    char send_buffer[SZ];
    ssize_t len;

    printf("Enter the movie tickets followed by number of seats: ");
    fflush(stdout);

    if (fgets(send_buffer, sizeof(send_buffer), stdin) == NULL)
    {
        perror("fgets error");
        return;
    }

    size_t input_len = strlen(send_buffer);
    if (input_len > 0 && send_buffer[input_len - 1] == '\n')
    {
        send_buffer[input_len - 1] = '\0';
    }

    ssize_t written = write(sockfd, send_buffer, strlen(send_buffer));
    if (written < 0)
    {
        perror("Write error");
        return;
    }

    memset(receive_buffer, 0, sizeof(receive_buffer));
    len = read(sockfd, receive_buffer, sizeof(receive_buffer) - 1);
    if (len < 0)
    {
        perror("Read error");
        return;
    }
    receive_buffer[len] = '\0';
    printf("Received: %s\n", receive_buffer);
}

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0)
    {
        perror("Invalid address/ Address not supported");
        return 1;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error");
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Connection failed");
        close(sockfd);
        return 1;
    }

    char receive_buffer[SZ];
    ssize_t n = read(sockfd, receive_buffer, sizeof(receive_buffer) - 1);
    if (n >= 0)
    {
        receive_buffer[n] = '\0';
        printf("%s", receive_buffer);
    }
    else
    {
        perror("Read error");
        close(sockfd);
        return 1;
    }

    while (1)
    {
        printf("Enter option:\n");
        fflush(stdout);

        char option_buffer[10];
        if (fgets(option_buffer, sizeof(option_buffer), stdin) == NULL)
        {
            perror("fgets error");
            close(sockfd);
            return 1;
        }

        size_t input_len = strlen(option_buffer);
        if (input_len > 0 && option_buffer[input_len - 1] == '\n')
        {
            option_buffer[input_len - 1] = '\0';
        }

        char ch = option_buffer[0];
        if (write(sockfd, &ch, 1) < 0)
        {
            perror("Write error");
            close(sockfd);
            return 1;
        }

        switch (ch)
        {
        case '1':
            display_option(sockfd);
            break;
        case '2':
            book_option(sockfd);
            break;
        case '3':
            cancel_option(sockfd);
            break;
        case '4':
            close(sockfd);
            return 0;
        default:
            printf("Oops, invalid option from the client.\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}
