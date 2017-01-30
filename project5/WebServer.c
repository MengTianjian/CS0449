#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MYPORT 50790

void *Process(void *p);
void Receive(int *connfd);
void Reply(int *connfd, char *file);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main()
{
	int sfd;
	struct sockaddr_in addr;
	sfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sfd == -1)
	{
		printf("Initialize socket error\n");
		return 1;
	}
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(MYPORT);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		printf("Bind port error\n");
		return 1;
	}
	int thread_id;
	pthread_t thread;
	int connfd;

	while (1)
	{
		if (listen(sfd, 10) == -1)
		{
			printf("Setup port error\n");
			return 1;
		}
		connfd = accept(sfd, NULL, NULL);
		thread_id = pthread_create(&thread, NULL, Process, &connfd);
		pthread_join(thread, NULL);
		close(connfd);
	}
	close(sfd);
	return 0;
}

void *Process(void *p)
{
	pthread_mutex_lock(&mutex);
	int *connfd = (int *)p;
	Receive(connfd);
	pthread_mutex_unlock(&mutex);
}

void Receive(int *connfd)
{
	char buf[1024];
	char file[1024];
	recv(*connfd, buf, 1024, 0);
	if (strncmp(buf, "GET ", 4) == 0)
	{
		int i;
		for (i = 0; i < 1024; i++)
		{
			if (buf[i+5] == ' ' || buf[i+5] == '\n' || buf[i+5] == '\0')
				break;
			file[i] = buf[i+5];
		}
		file[i] = '\0';
		Reply(connfd, file);
	}
	FILE *f = fopen("stats.txt", "a");
	fprintf(f, "\n%s\n", buf);
	fclose(f);
	memset(buf, 0, sizeof(buf));
}

void Reply(int *connfd, char *file)
{
	FILE *f;
	f = fopen(file, "r");
	char *message;
	if (f == NULL)
	{
		message = (char *)malloc(25*sizeof(char));
		strcpy(message, "HTTP/1.1 404 Not Found\n");
	}
	else
	{
		fseek(f, 0, SEEK_END);
		int length = (int)ftell(f);
		fseek(f, 0, SEEK_SET);
		message = (char *)malloc((200+length)*sizeof(char));
		strcpy(message, "HTTP/1.1 200 OK\n");
		time_t curtime;
		struct tm *loctime;
		curtime = time(NULL);
		loctime = localtime(&curtime);
		char strtime[100];
		strftime(strtime, 100, "Date: %A, %d %B %Y %X %Z\n", loctime);
		strcat(message, strtime);
		char c[100];
		sprintf(c, "Content-Length: %d\nConnection: close\nContent-Type: text/html\n\n", length);
		strcat(message, c);
		char *filecontent = (char *)malloc(length*sizeof(char));
		fread(filecontent, 1, length, f);
		strcat(message, filecontent);
		fclose(f);
		free(filecontent);
	}
	send(*connfd, message, strlen(message), 0);
	free(message);
}
