#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 4000
#define WS 5

typedef struct
{
    int fno;
    char c;
} block;

block message[WS];
int clientfd;
int current_window_start = 0;
int skip_count = 0;

void create_input(int start)
{
    for (int i = 0; i < WS; ++i)
    {
        message[i].fno = start + i;
        message[i].c = 'A' + (start + i) % 26;
    }
}

void send_window(int start, int skip_frame)
{
    for (int i = 0; i < WS; ++i)
    {
        if (i != skip_frame)
        {
            char str[3];
            sprintf(str, "%d%c", message[i].fno, message[i].c);
            write(clientfd, str, strlen(str));
            printf("Sent: %d%c\n", message[i].fno, message[i].c);
        }
        else
        {
            printf("Skipped sending frame: %d%c\n", message[i].fno, message[i].c);
        }
        sleep(1);
    }
}

void *read_ack(void *args)
{
    while (1)
    {
        char buff[2];
        ssize_t len = read(clientfd, buff, sizeof(buff) - 1);
        buff[len] = '\0';

        if (buff[0] == -1)
        {
            printf("Client finished receiving.\n");
            break;
        }

        int ack = buff[0] - '0';
        printf("Received ACK: %d\n", ack);

        if (ack < WS)
        {
            printf("Resending from frame %d due to ACK\n", ack);
            send_window(ack, -1);
        }
        else
        {
            current_window_start += WS;
            create_input(current_window_start);

            if (current_window_start >= 26)
            {
                char end = -1;
                write(clientfd, &end, sizeof(end));
                break;
            }

            if (skip_count < 2) 
            {
                send_window(0, current_window_start % WS); 
                skip_count++;
            }
            else
            {
                send_window(0, -1);  
            }
        }
    }
    return NULL;
}

int main()
{
    struct sockaddr_in servaddr;

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    listen(sockfd, 2);
    clientfd = accept(sockfd, NULL, NULL);

    create_input(current_window_start);
    send_window(0, -1);  
    pthread_t rthread;
    pthread_create(&rthread, NULL, read_ack, NULL);

    pthread_join(rthread, NULL);

    close(sockfd);
    return 0;
}
