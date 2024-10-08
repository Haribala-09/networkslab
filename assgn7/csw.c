#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/select.h>

#define PORT 4000
#define WS 5
#define TIMEOUT 2 // Timeout in seconds

int sockfd;
int expected_frame = 0;
int missed_frame = -1;

void send_ack()
{
    char buff[2];
    if (missed_frame == -1)
    {
        buff[0] = WS + '0';
    }
    else
    {
        buff[0] = missed_frame + '0';
    }
    write(sockfd, buff, 1);
    printf("Sent ACK: %d\n", missed_frame != -1 ? missed_frame : WS);
}

void *read_mesg(void *args)
{
    char buff[3];
    int window_received = 0;
    int timeout_occurred = 0;

    while (window_received < WS)
    {
        fd_set read_fds;
        struct timeval timeout;
        
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);
        
        timeout.tv_sec = TIMEOUT;
        timeout.tv_usec = 0;

        int select_result = select(sockfd + 1, &read_fds, NULL, NULL, &timeout);
        if (select_result <= 0) // Timeout or error
        {
            timeout_occurred = 1;
            break;
        }

        ssize_t len = read(sockfd, buff, sizeof(buff) - 1);
        if (len == 0 || buff[0] == -1)
        {
            printf("Server finished sending.\n");
            return NULL;
        }

        buff[len] = '\0';
        int fno = buff[0] - '0';

        printf("Received frame: %d%c\n", fno, buff[1]);

        if (fno == expected_frame)
        {
            expected_frame++;
        }
        else
        {
            if (missed_frame == -1 || fno < missed_frame)
            {
                missed_frame = expected_frame;
            }
        }

        window_received++;
    }

    if (timeout_occurred)
    {
        printf("Timeout occurred. Sending ACK for missed frame %d\n", missed_frame != -1 ? missed_frame : WS);
        send_ack();
    }
    else
    {
        send_ack();
    }

    missed_frame = -1;

    return NULL;
}

int main()
{
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    while (1)
    {
        pthread_t recv_thread;
        pthread_create(&recv_thread, NULL, read_mesg, NULL);
        pthread_join(recv_thread, NULL);
    }

    close(sockfd);
    return 0;
}
