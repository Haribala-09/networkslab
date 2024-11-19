#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define MAX_LEN 100

const int ports[] = {1373, 1374, 1375};
const char *groups[] = {"224.0.1.1", "224.0.1.2", "224.0.1.3"};

void *receive_message(void *arg)
{
    int id = *(int *)arg;
    free(arg);

    int s;
    struct sockaddr_in srv, cli;
    struct ip_mreq mreq;
    char buf[MAX_LEN];
    socklen_t n;

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        pthread_exit(NULL);
    }

    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_port = htons(ports[id]);
    if (inet_pton(AF_INET, groups[id], &srv.sin_addr) < 0)
    {
        perror("inet_aton");
        close(s);
        pthread_exit(NULL);
    }

    if (bind(s, (struct sockaddr *)&srv, sizeof(srv)) < 0)
    {
        perror("bind");
        close(s);
        pthread_exit(NULL);
    }

    if (inet_aton(groups[id], &mreq.imr_multiaddr) < 0)
    {
        perror("inet_aton");
        close(s);
        pthread_exit(NULL);
    }
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    if (setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        perror("setsockopt");
        close(s);
        pthread_exit(NULL);
    }

    n = sizeof(cli);
    while (1)
    {
        int r = recvfrom(s, buf, MAX_LEN, 0, (struct sockaddr *)&cli, &n);
        if (r < 0)
        {
            perror("recvfrom");
        }
        else
        {
            buf[r] = 0;
            printf("Group %d: %s\n", id, buf);
        }
    }

    close(s);
    pthread_exit(NULL);
}

int main()
{
    pthread_t receiver_threads[3];

    for (int i = 0; i < 3; i++)
    {
        int *id = malloc(sizeof(int));
        *id = i;
        if (pthread_create(&receiver_threads[i], NULL, receive_message, id) != 0)
        {
            perror("pthread_create");
            return 1;
        }
    }

    for (int i = 0; i < 3; i++)
    {
        pthread_join(receiver_threads[i], NULL);
    }

    return 0;
}
