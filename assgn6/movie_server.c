#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 5000

const int msize = 3;
const int max_size = 1000;
const int sz = 200;
const int max_client = 6;
const int total_seats = 20;

pthread_mutex_t lock;

typedef struct
{
	char *movie_name;
	uint16_t seats;
} movies;

movies *arr;

void create_movies()
{
	arr = malloc(sizeof(movies) * msize);
	if (arr == NULL)
	{
		perror("Failed to allocate memory for movies");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < msize; i++)
	{
		arr[i].movie_name = malloc(sz);
		arr[i].seats = 20;
	}

	strcpy(arr[0].movie_name, "Beautiful Mind");
	strcpy(arr[1].movie_name, "Schindler's List");
	strcpy(arr[2].movie_name, "Wolf of Wall Street");
	printf("%s", arr[2].movie_name);
}

char *options()
{
	char *opt = malloc(sz);
	if (opt == NULL)
	{
		perror("Failed to allocate memory for options");
		exit(EXIT_FAILURE);
	}
	snprintf(opt, sz, "1. List of movies and available seats\n2. Book a ticket for a movie\n3. Cancel a movie ticket\n4. Exit\n");
	return opt;
}

char *get_movie_list()
{
	char *movie_list = malloc(sz * msize);
	if (movie_list == NULL)
	{
		perror("Failed to allocate memory for movie list");
		exit(EXIT_FAILURE);
	}
	movie_list[0] = '\0';
	for (int i = 0; i < msize; i++)
	{
		char buffer[64];
		snprintf(buffer, sizeof(buffer), "%s - %u\n", arr[i].movie_name, arr[i].seats);
		strncat(movie_list, buffer, sz * msize - strlen(movie_list) - 1);
	}
	return movie_list;
}

int strtoint(const char *str)
{
	int num = 0;
	while (*str >= '0' && *str <= '9')
	{
		num = num * 10 + (*str - '0');
		str++;
	}
	return num;
}

int book_tickets(const char *request)
{
	int seat_count = strtoint(request + 2);
	int movie_index = request[0] - '0';

	if (movie_index < 0 || movie_index >= msize || seat_count <= 0 || seat_count > arr[movie_index].seats)
		return -1;
	pthread_mutex_lock(&lock);
	arr[movie_index].seats -= seat_count;
	pthread_mutex_unlock(&lock);
	return seat_count;
}

int cancel_tickets(const char *request)
{
	int seat_count = strtoint(request + 2);
	int movie_index = request[0] - '0';

	if (movie_index < 0 || movie_index >= msize || seat_count <= 0)
		return -1;
	if (seat_count + arr[movie_index].seats > total_seats)
		return -1;

	pthread_mutex_lock(&lock);
	arr[movie_index].seats += seat_count;
	pthread_mutex_unlock(&lock);

	return seat_count;
}

void display_option(int clientfd)
{
	char *movie_info = get_movie_list();
	write(clientfd, movie_info, strlen(movie_info));
	free(movie_info);
}

void book_option(int clientfd)
{
	char receive_buffer[10];
	ssize_t len = read(clientfd, receive_buffer, sizeof(receive_buffer) - 1);
	if (len < 0)
	{
		perror("Error reading booking info");
		return;
	}
	receive_buffer[len] = '\0';

	int c = book_tickets(receive_buffer);
	if (c > 0)
	{
		char message[sz];
		snprintf(message, sizeof(message), "%d tickets booked for %s\n", c, arr[receive_buffer[0] - '0'].movie_name);
		write(clientfd, message, strlen(message));
	}
	else
	{
		char *err_message = "Number of seats requested is invalid or exceeds available seats.\n";
		write(clientfd, err_message, strlen(err_message));
	}
}

void cancel_option(int clientfd)
{
	char receive_buffer[10];

	ssize_t len = read(clientfd, receive_buffer, sizeof(receive_buffer) - 1);
	if (len < 0)
	{
		perror("Error reading cancellation info");
		return;
	}
	receive_buffer[len] = '\0';

	int c = cancel_tickets(receive_buffer);
	char message[sz];
	if (c >= 0)
	{
		snprintf(message, sizeof(message), "%d tickets cancelled for %s\n", c, arr[receive_buffer[0] - '0'].movie_name);
	}
	else
	{
		snprintf(message, sizeof(message), "Invalid movie number or higher seat count or cancellation request.\n");
	}
	write(clientfd, message, strlen(message));
}

void *client_booking(void *args)
{
	int clientfd = *(int *)args;
	free(args); // Free the allocated memory for clientfd

	char *opt = options();
	write(clientfd, opt, strlen(opt));
	free(opt);

	while (1)
	{
		char ch;
		ssize_t len = read(clientfd, &ch, 1);
		if (len <= 0)
		{
			perror("Error reading from client or client disconnected");
			break;
		}

		switch (ch)
		{
		case '1':
			display_option(clientfd);
			break;
		case '2':
			book_option(clientfd);
			break;
		case '3':
			cancel_option(clientfd);
			break;
		case '4':
			close(clientfd);
			return NULL;
		default:
			printf("Oops, invalid option from the client.\n");
			break;
		}
	}
	close(clientfd);
	return NULL;
}

void *accept_clients(void *args)
{
	pthread_t tid;
	int sockfd = *(int *)args;
	for (int i = 0; i < max_client; i++)
	{
		int *clientfd = malloc(sizeof(int));
		if (clientfd == NULL)
		{
			perror("Failed to allocate memory for clientfd");
			exit(EXIT_FAILURE);
		}

		*clientfd = accept(sockfd, NULL, NULL);
		if (*clientfd < 0)
		{
			free(clientfd);
			perror("Connection failed");
			continue;
		}

		printf("Connection established with client fd: %d\n", *clientfd);
		pthread_create(&tid, NULL, client_booking, clientfd);
		pthread_detach(tid);
	}

	return NULL;
}

signed main()
{

	if (pthread_mutex_init(&lock, NULL) != 0)
	{
		perror("Mutex init failed\n");
		return EXIT_FAILURE;
	}
	// Server declarations
	struct sockaddr_in servaddr;
	int sockfd;

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("Socket creation failed");
		return EXIT_FAILURE;
	}

	if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("Bind failed");
		close(sockfd);
		return EXIT_FAILURE;
	}

	if (listen(sockfd, max_client) < 0)
	{
		perror("Listen failed");
		close(sockfd);
		return EXIT_FAILURE;
	}

	pthread_t tid;
	create_movies();
	pthread_create(&tid, NULL, accept_clients, &sockfd);
	pthread_join(tid, NULL);

	close(sockfd);
	for (int i = 0; i < msize; i++)
	{
		free(arr[i].movie_name);
	}
	free(arr);

	return 0;
}
