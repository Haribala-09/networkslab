#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <netinet/in.h>
#include <ctype.h>

const uint16_t maxn = 5;
const uint16_t MAX_QUESTION_LENGTH = 256;

typedef struct questions
{
	char *arr;
	uint8_t marks;
	char correct_option;
	bool iscorrect;
} questions;

questions *create()
{
	questions *ques = (questions *)malloc(maxn * sizeof(questions));
	if (!ques)
	{
		printf("Memory allocation failed\n");
		exit(1);
	}

	for (int i = 0; i < maxn; i++)
	{
		ques[i].arr = (char *)malloc(MAX_QUESTION_LENGTH * sizeof(char));
		if (!ques[i].arr)
		{
			printf("Memory allocation for question string failed\n");
			exit(1);
		}
	}

	return ques;
}

char *check_answer(questions *ques, char *read_buffer, int idx)
{
	if (tolower(read_buffer[0]) == tolower(ques[idx].correct_option))
	{
		return "correct";
	}

	ques[idx].iscorrect = false;
	return "wrong";
}

char *calculate_result(questions *ques)
{
	uint16_t ans = 0;

	for (int i = 0; i < maxn; i++)
	{
		ans += (ques[i].iscorrect ? ques[i].marks : 0);
	}

	char *res = malloc(30);
	snprintf(res, 30, "Your score is: %d / 10", ans);
	return res;
}
void freeques(questions *ques)
{
	for (int i = 0; i < maxn; i++)
	{
		free(ques[i].arr);
	}
	free(ques);
}
void initialize_questions(questions *ques)
{
	strcpy(ques[0].arr, "Q1: What is 2+2?\nA) 3\nB) 4\nC) 5\nD) 6");
	ques[0].marks = 2;
	ques[0].correct_option = 'b';
	ques[0].iscorrect = true;

	strcpy(ques[1].arr, "Q2: What is the capital of France?\nA) Berlin\nB) Madrid\nC) Paris\nD) Rome");
	ques[1].marks = 2;
	ques[1].correct_option = 'c';
	ques[1].iscorrect = true;

	strcpy(ques[2].arr, "Q3: Which planet is known as the Red Planet?\nA) Earth\nB) Venus\nC) Mars\nD) Jupiter");
	ques[2].correct_option = 'c';
	ques[2].marks = 2;
	ques[2].iscorrect = true;

	strcpy(ques[3].arr, "Q4: Who wrote 'Hamlet'?\nA) Charles Dickens\nB) J.K. Rowling\nC) William Shakespeare\nD) Mark Twain");
	ques[3].correct_option = 'c';
	ques[3].marks = 2;
	ques[3].iscorrect = true;

	strcpy(ques[4].arr, "Q5: What is the boiling point of water?\nA) 100°C\nB) 90°C\nC) 110°C\nD) 120°C");
	ques[4].correct_option = 'a';
	ques[4].marks = 2;
	ques[4].iscorrect = true;
}

signed main()
{
	char read_buffer[20]; 
	int listenfd, len;
	struct sockaddr_in servaddr, cliaddr;
	memset(&servaddr, 0, sizeof(servaddr));

	listenfd = socket(AF_INET, SOCK_DGRAM, 0);

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5000);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(listenfd, (struct sockaddr *)&servaddr, sizeof servaddr);

	len = sizeof(cliaddr);
	int n = recvfrom(listenfd, read_buffer, sizeof(read_buffer),
					 0, (struct sockaddr *)&cliaddr, &len);
	read_buffer[n] = '\0';
	puts(read_buffer);

	questions *ques = create();
	initialize_questions(ques);

	for (int i = 0; i < maxn; i++)
	{
		memset(read_buffer, 0, sizeof(read_buffer));
		sendto(listenfd, ques[i].arr, strlen(ques[i].arr), 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));

		int sz = recvfrom(listenfd, read_buffer, sizeof(read_buffer), 0, (struct sockaddr *)&cliaddr, &len);
		read_buffer[sz] = '\0';

		char *ans = check_answer(ques, read_buffer, i);
		sendto(listenfd, ans, strlen(ans), 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
	}

	char *final_score = calculate_result(ques);
	sendto(listenfd, final_score, strlen(final_score), 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));

	freeques(ques);
	free(final_score);
	close(listenfd);
	return 0;
}
