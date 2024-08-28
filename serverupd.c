#include<stdio.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/tpes.h>
#include<string.h>



int main(){
	char read_buffer[100],write_buffer[100];
	int listenfd,len;
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));

	listenfd=socket(AF_INET,SOCK_DGRAM,0);

	//Creating a servaddr of type ipv4 and port 3200 with taking any inbuild ip as specified as INADDR_ANY
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(3200);
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);

	bind(listenfd,(struct sockaddr*)&servaddr,sizeof servaddr);



}
