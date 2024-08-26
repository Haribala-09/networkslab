#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

int main() {
    int sockfd;
    char send_buffer[100];
    char receive_buffer[100];
    struct sockaddr_in cliaddr;
    
    memset(&cliaddr, 0, sizeof(cliaddr));
    
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(1800);
    
    if (inet_pton(AF_INET, "127.0.0.1", &(cliaddr.sin_addr)) <= 0) {
        perror("Invalid address/ Address not supported");
        return 1;
    }
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return 1;
    }
    
    if (connect(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        return 1;
    }
    
    while (1) {
        memset(send_buffer, 0, sizeof(send_buffer));
        memset(receive_buffer, 0, sizeof(receive_buffer));
        
       
       	if (fgets(send_buffer, sizeof(send_buffer), stdin) == NULL) {
            perror("Input error");
            close(sockfd);
            return 1;
        }
        
       // send_buffer[strcspn(send_buffer, "\n")] = '\0';
        
        if (strcmp(send_buffer, "end") == 0) {
            write(sockfd, send_buffer, strlen(send_buffer) + 1);
            ssize_t read_len=read(sockfd, receive_buffer, sizeof(receive_buffer) - 1);
            receive_buffer[read_len] = '\0';
            printf("%s\n", receive_buffer);
            break;
        }
        
        
        write(sockfd, send_buffer, strlen(send_buffer) + 1);
        
     
        ssize_t read_size = read(sockfd, receive_buffer, sizeof(receive_buffer) - 1);
        if (read_size < 0) {
            perror("Read error");
            close(sockfd);
            return 1;
        }
        receive_buffer[read_size] = '\0';
        
        printf("%s\n", receive_buffer);
    }
    
    close(sockfd);
    return 0;
}

