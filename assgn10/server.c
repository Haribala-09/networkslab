#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>

#define max_len 100
const int n = 3;

const int port[] = {1373, 1374, 1375};
const char *groups[] = {"224.0.1.1", "224.0.1.2", "224.0.1.3"};
const char *message[] = {"8.00", "8.30", "9.00"};

#ifndef HBK_DEBUG
void dbg_out(int x)
{
    fprintf(stderr, "%d\n", x);
}

#define dbg(x)                                         \
    fprintf(stderr, "[Line:%d] (" #x "): ", __LINE__); \
    dbg_out(x)

#else
#define dbg(x)
#endif

void *handle(void *args)
{
    int id = *(int *)args;
    int s;
    struct sockaddr_in srv;

    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_port = htons(port[id]);
    if (inet_pton(AF_INET, groups[id], &srv.sin_addr) < 0)
    {
        perror("inet_aton");
        pthread_exit(NULL);
    }
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        pthread_exit(NULL);
    }

    while (true)
    {
        sendto(s, message[id], strlen(message[id]), 0, (struct sockaddr *)&srv, sizeof(srv));
        sleep(3);
    }

    close(s);
}

int main()
{
#ifndef HBK_DEBUG
    freopen("input.txt", "r", stdin);
    freopen("error.txt", "w", stderr);
#endif

    pthread_t pid[3];
    int *arr = malloc(sizeof(int) * 3);
    for (int i = 0; i < 3; i++)
        arr[i] = i;

    for (int i = 0; i < n; i++)
    {
        pthread_create(&pid[i], NULL, handle, &arr[i]);
    }

    for (int i = 0; i < 3; i++)
    {
        pthread_join(pid[i], NULL);
    }

    free(arr);
    return 0;
}