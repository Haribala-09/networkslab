#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>   

void manipulate(const char* re, char* send) {
    int rn = strlen(re);

    if (re[0] >= '0' && re[0] <= '9') {
        int i = 0, num[2];
        for (int k = 0; k < 2; k++) {
            num[k] = 0;
            while (i < rn && re[i] != ' ') {
                num[k] = num[k] * 10 + (re[i] - '0');
                i++;
            }
            i++;  // Skip space
        }
        int sum = num[0] + num[1];
        sprintf(send, "%d", sum);  // remember this
    } else if (re[0] == 'b') {
        if (strstr(re, "001")) {
            strcpy(send, "001 is present in the binary string");
        } else {
            strcpy(send, "001 is not present in the binary string");
        }
    } else {
        int l = 0, r = rn - 1;
        while (l < r) {
            if (re[l] != re[r]) {
                strcpy(send, "Given string is not a palindrome");
                return;
            }
            l++;
            r--;
        }
        strcpy(send, "Given string is a palindrome");
    }
}

int main() {
    int sockfd, client1fd;
    struct sockaddr_in servaddr;

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(2000);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        return 1;
    }

    if (listen(sockfd, 1) < 0) {
        perror("Listen failed");
        close(sockfd);
        return 1;
    }

    client1fd = accept(sockfd, NULL, NULL);
    if (client1fd < 0) {
        perror("Accept failed");
        close(sockfd);
        return 1;
    }

    char send_buffer[100];
    char receive_buffer[100];
    fflush(stdout);
    printf("Server is starting\n");
    while (1) {
        memset(send_buffer, 0, sizeof(send_buffer));
        memset(receive_buffer, 0, sizeof(receive_buffer));
        
        ssize_t read_size = read(client1fd, receive_buffer, sizeof(receive_buffer) - 1);
        if (read_size < 0) {
            perror("Read failed");
            break;
        }
        receive_buffer[read_size] = '\0';  // Null-terminate the string

        printf("Tmp:%c\n", receive_buffer[0]);
	if (strcmp(receive_buffer, "end") == 0) {
            const char* str = "Connection closed";
            write(client1fd, str, strlen(str) + 1);
            break;
        }
        
	manipulate(receive_buffer, send_buffer);
        write(client1fd, send_buffer, strlen(send_buffer) + 1);
    }

    close(client1fd);
    close(sockfd);
    return 0;
}

