#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define int uint64_t

bool data_parity(int val)
{
    int o = 0, z = 0;

    while (val)
    {
        o += (val & 1);
        z += (!(val & 1));
        val >>= 1;
    }
    printf("O:%llu\n", o);
    printf("Z:%llu\n", z);

    return !(o & 1);
}

signed main()
{
    int sockfd;
    struct sockaddr_in cliaddr;

    memset(&cliaddr, 0, sizeof(cliaddr));

    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(1800);

    if (inet_pton(AF_INET, "127.0.0.1", &(cliaddr.sin_addr.s_addr)) <= 0)
    {
        perror("Invalid address/ Address not supported");
        return 1;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error");
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr)) < 0)
    {
        perror("Connection failed");
        close(sockfd);
        return 1;
    }

    for(int i=0;i<3;i++)
    {
        int val;
        ssize_t rs=read(sockfd,&val,sizeof(val));

        bool check=data_parity(val);

        write(sockfd,&check,sizeof(check));

    }

    close(sockfd);
    return 0;
}