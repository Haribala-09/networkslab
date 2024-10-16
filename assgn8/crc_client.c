#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define PORT 4500

signed main()
{
    struct sockaddr_in cliaddr;

    cliaddr.sin_port=PORT;
    
}