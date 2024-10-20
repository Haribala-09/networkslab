#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>

#define int uint64_t

int binary_to_num(char *str)
{
    int res = 0;
    size_t k = 0;
    while (k < strlen(str) && str[k] == '0')
        k++;

    int cnt = 0;
    for (size_t i = strlen(str) - 1; i >= k; i--)
    {
        if (str[i] == '1')
        {
            res += (1 << cnt);
        }
        cnt++;
    }
    return res;
}

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
    // padded with zero to match the bits of the dividend
    num_len--;
    int diff_len = num_len - divisor_len + 1;
    int pad_divisor = divisor << diff_len;
    int cpy = pad_divisor;
    // printf("len%lu\n", diff_len);
    // printf("pd%lu\n", pad_divisor);
    // printf("num%lu\n", num);

    while (pad_divisor >= divisor)
    {
        num = num ^ pad_divisor;
        printf("n%lu\n", num);

        // Right shift the divisor until it matches with the highest set bit of the dividend
        while (!(num >> num_len))
        {
            pad_divisor >>= 1;
            // num>>=1;
            printf("d%lu\n", pad_divisor);
            num_len--;
        }
    }
    return num;
}

signed main()
{
#ifndef HBK_DEBUG
    freopen("input.txt", "r", stdin);
#endif
    int num = 13548;
    int divisor = 11;
    int cpy_num=num;
    // Left shift the dividend by zeros like padding with zeros of crc polynomial length
    num <<= 3;
    int num_len = find_len(num);
    int divisor_len = find_len(divisor);
    int remainder = crc(num, divisor, divisor_len, num_len);

    printf("Reminder: %lu\n",remainder);
    int sent_data = (cpy_num << divisor_len) | remainder;
    printf("Number: %lu\n",cpy_num);
    
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Socket creation failed...\n");
        exit(0);
    }
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("Connection with the server failed...\n");
        exit(0);
    }

    write(sockfd, &sent_data, sizeof(sent_data));
    printf("Sent data: %lu\n", sent_data);

    close(sockfd);
}