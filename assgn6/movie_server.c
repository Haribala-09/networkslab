#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<string.h>
#include<pthread.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>

#define PORT 5000
const int msize=3;
const int max_size=1000;
const int sz=100;



typedef struct{
	char* movie_name;
	uint_16 seats;
}movies;


void create_movies(movies* arr){
	
	for(int i=0;i<msize;i++){
		arr[i]->movie_name=malloc(30);
		arr[i]->seats=20;
	}

	strcpy(arr[0]->movie_name,"Beautiful Mind");
	strcpy(arr[1]->movie_name,"Schindlers List");
	strcpy(arr[2]->movie_name,"Wolf of Wall Street");
}

inline void options(char* opt){
	opt=(char*)malloc(sz);
	strcpy(opt,"1.List of movies and available seats\n2.Book a ticket for a movie\n3.Cancel a movie ticket\n");
}
inline void get_movie_list(char* movie_list,const movies* arr)
{
	movie_list=(char*)malloc(sz);
	
	for(int i=0;i<msize;i++)
	{
		strcat(movie_list,arr[i]->movie_name);
		char buffer[5];
		sprintf(buffer,"%d",arr[i]->seats);
		strcat(movie_list," - ");
		strcat(movie_list,buffer);
		strcat(movie_list,"\n");
	}
}
void accept_clients(){
	
}
int main(){
	// Server declarations
	int sockfd;
	struct sockaddr_in serveraddr;

	memset(&servaddr,0,sizeof servaddr);

	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(PORT);
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);

	sockfd=socket(AF_INET,SOCK_STREAM,0);

	if(sockfd<0)
	{
		perror("Socket creation failed");
		return EXIT_FAILURE;
	}

	if(bind(sockfd,(struct sockaddr*)&servaddr,sizeof servaddr)<0)
	{
		perror("Bind failed");
		return EXIT_FAILURE;
	}

	if(listen(sockfd,7)<0)
	{
		perror("Listen Failed");	
		return EXIT_FAILURE;
	}

	movies arr[msize];
	char* options;
	// Function calls 
	
}
