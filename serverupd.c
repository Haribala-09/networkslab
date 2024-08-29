#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

int main()
{
	char read_buffer[100], write_buffer[100];
	int listenfd, len;
	struct sockaddr_in servaddr,cliaddr;
	memset(&servaddr, 0, sizeof(servaddr));

	listenfd = socket(AF_INET, SOCK_DGRAM, 0);

	// Creating a servaddr of type ipv4 and port 3200 with taking any inbuild ip as specified as INADDR_ANY
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5000);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(listenfd, (struct sockaddr *)&servaddr, sizeof servaddr);

	len = sizeof(cliaddr);
	int n = recvfrom(listenfd, read_buffer, sizeof(read_buffer),
					 0, (struct sockaddr *)&cliaddr, &len); // receive message from server
	read_buffer[n] = '\0';
	puts(read_buffer);

	sendto(listenfd, "This life sucks!", sizeof(write_buffer), 0,
		   (struct sockaddr *)&cliaddr, sizeof(cliaddr));
}
