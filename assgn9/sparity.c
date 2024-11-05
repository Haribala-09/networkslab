#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define int uint64_t

int data_parity(int val)
{
    int cpy = val, o = 0, z = 0;

    while (cpy)
    {
        o += (cpy & 1);
        z += (!(cpy & 1));
        cpy >>= 1;
    }
    printf("O:%llu\n", o);
    printf("Z:%llu\n", z);

    // Adding the parity bit at the last zuckkk it
    return (val << 1) | (o & 1);
}

signed main()
{
#ifndef HBK_DEBUG
    freopen("output.txt", "w", stdout);
    setvbuf(stdout, NULL, _IONBF, 0);
#endif

    // int num = 47;
    // printf("DP:%d\n", data_parity(num));

    int sockfd, client1fd;
    struct sockaddr_in servaddr;

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(1800);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Bind failed");
        close(sockfd);
        return 1;
    }

    if (listen(sockfd, 1) < 0)
    {
        perror("Listen failed");
        close(sockfd);
        return 1;
    }

    client1fd = accept(sockfd, NULL, NULL);

    if (client1fd < 0)
    {
        perror("Accept failed");
        close(sockfd);
        return 1;
    }

    for (int i = 0; i < 3; i++)
    {
        int val;
        // printf("Enter a number: ");
        scanf("%llu", &val);

        val = data_parity(val);
        printf("Value with parity bit: %llu\n", val);

        if (i == 1)
        {
            val ^= (1ULL << 6);
        }
        else if (i == 2)
        {
            val ^= (1ULL << 6);
            val ^= (1ULL << 8);
        }

        printf("Value with parity bit after change: %llu\n", val);

        write(client1fd, &val, sizeof(val));

        bool rval;
        ssize_t read_size = read(client1fd, &rval, sizeof(rval));

        if (read_size < 0)
        {
            perror("Read failed");
            break;
        }

        if (rval)
        {
            printf("The data is not manipulated, the parity check is verified\n");
        }
        else
        {
            printf("The data is changed, and the parity check failed\n");
        }
    }

    close(client1fd);
    close(sockfd);
    return 0;
}