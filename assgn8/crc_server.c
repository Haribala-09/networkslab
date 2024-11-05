#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>

#define int uint64_t

int find_len(uint64_t num)
{
    int len = 0;
    while (num)
    {
        len++;
        num >>= 1;
    }
    return len;
}

int crc(int num, int divisor, int divisor_len, int num_len)
{
    num_len--;
    int diff_len = num_len - divisor_len + 1;
    int pad_divisor = divisor << diff_len;

    while (pad_divisor >= divisor)
    {
        num = num ^ pad_divisor;

        while (!(num >> num_len))
        {
            pad_divisor >>= 1;
            num_len--;
        }
    }
    return num;
}

signed main()
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Socket creation failed...\n");
        exit(0);
    }
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(8080);

    if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("Socket bind failed...\n");
        exit(0);
    }

    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }

    socklen_t len = sizeof(cli);
    connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
    if (connfd < 0)
    {
        printf("Server accept failed...\n");
        exit(0);
    }

    int received_data;
    read(connfd, &received_data, sizeof(received_data));

    printf("Received data: %lu\n", received_data);

    int divisor = 11;
    int divisor_len = find_len(divisor);

    // Remove the crc and then take the message
    int original_num = received_data >> divisor_len;

    // Take the crc alone by doing and with 0s of message length
    int received_crc = received_data & ((1 << divisor_len) - 1);

    printf("Original number: %lu\n", original_num);
    printf("Received CRC: %lu\n", received_crc);

    original_num <<= 3;
    int original_len = find_len(original_num);
    int calculated_crc = crc(original_num, divisor, divisor_len, original_len);

    if (calculated_crc == received_crc)
    {
        printf("CRC check passed.\n");
    }
    else
    {
        printf("CRC check failed. Expected: %lu, Got: %lu\n", calculated_crc, received_crc);
    }

    close(sockfd);
}
