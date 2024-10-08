#include "stack.h"
#include "stackd.h"
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

bool check_operator(char a)
{
    return a == '*' || a == '+' || a == '-' || a == '/';
}

bool check_number(char a)
{
    return a >= '0' && a <= '9';
}

int check_priority(char a)
{
    if (a == '+' || a == '-')
        return 1;
    if (a == '*' || a == '/')
        return 2;
    return 0;
}

double compute(char ch, double a, double b)
{
    switch (ch)
    {
    case '+':
        return a + b;
    case '-':
        return a - b;
    case '*':
        return a * b;
    case '/':
        return a / b;
    }
    return 0.0;
}

void evaluate(const char *re, char *send)
{
    const int n = strlen(re);
    stack_double *numst = (stack_double *)malloc(sizeof(stack_double));
    create_double(numst);
    stack *opst = (stack *)malloc(sizeof(stack));
    create(opst);

    for (int i = 0; i < n; i++)
    {
        if (check_number(re[i]))
        {
            double val = 0.0;
            while (check_number(re[i]))
            {
                val = val * 10 + (re[i] - '0');
                i++;
            }
            i--;
            push_double(numst, val);
        }
        else if (check_operator(re[i]))
        {
            while (!empty(opst) && check_priority(re[i]) <= check_priority(top(opst)))
            {
                char top_op = pop(opst);
                double d1 = pop_double(numst);
                double d2 = pop_double(numst);
                push_double(numst, compute(top_op, d2, d1));
            }
            push(opst, re[i]);
        }
    }

    while (!empty(opst))
    {
        char top_op = pop(opst);
        double d1 = pop_double(numst);
        double d2 = pop_double(numst);
        push_double(numst, compute(top_op, d2, d1));
    }

    double result = pop_double(numst);
    snprintf(send, 100, "%.3f", result);

    free(numst->arr);
    free(numst);
    free(opst->arr);
    free(opst);
}

int main()
{
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

    char send_buffer[100];
    char receive_buffer[100];

    while (true)
    {
        memset(send_buffer, 0, sizeof(send_buffer));
        memset(receive_buffer, 0, sizeof(receive_buffer));

        ssize_t read_size = read(client1fd, receive_buffer, sizeof(receive_buffer) - 1);

        if (read_size < 0)
        {
            perror("Read failed");
            break;
        }
        receive_buffer[read_size] = '\0';
        puts(receive_buffer);
        if (strcmp(receive_buffer, "end") == 0)
        {
            const char *str = "Connection closed";
            write(client1fd, str, strlen(str) + 1);
            break;
        }

        evaluate(receive_buffer, send_buffer);
        write(client1fd, send_buffer, strlen(send_buffer) + 1);
    }

    close(client1fd);
    close(sockfd);
    return 0;
}
