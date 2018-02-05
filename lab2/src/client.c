#include "cast.h"
#include "common.h"
#include "debug.h"
#include "defines.h"
#include "timer.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

static char readFormat[] = "r%d\n";
static char writeFormat[] = "w%d\n";

static char connectionSemaphoreName[] = "/420.client.connection";
static sem_t* connectionSemaphore;

static int port;
static int count;
int socketLimit;

void* connectToServer(void* data) {
	int rank = VOIDP2INT(data);
	unsigned int seed = rank;
	
	int targetIndex = rand_r(&seed) % count;
	int readWrite = rand_r(&seed) % 100;
	
	struct sockaddr_in sock_var;
	sock_var.sin_addr.s_addr = inet_addr(localhost);
	sock_var.sin_port = port;
	sock_var.sin_family = AF_INET;
	
	sem_wait(connectionSemaphore);
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == -1) {
		perror("socket()");
		exit(EXIT_FAILURE);
	}
	if (connect(clientSocket, (struct sockaddr*)&sock_var, sizeof(sock_var)) == -1) {
		perror("connect()");
		exit(EXIT_FAILURE);
	}
	
	Print("Thread %d: index=%d, write=%d\n", rank, targetIndex, readWrite>=95);
	char sendString[REQUEST_LENGTH], receiveString[RESPONSE_LENGTH];
	char* format = readWrite >= 95 ? writeFormat : readFormat;
	snprintf(sendString, 10, format, targetIndex);
	int writeResult = write(clientSocket, sendString, REQUEST_LENGTH);
	if (writeResult == -1) {
		perror("write()");
		exit(EXIT_FAILURE);
	}
	int readResult = read(clientSocket, receiveString, RESPONSE_LENGTH);
	if (readResult == -1) {
		perror("read()");
		exit(EXIT_FAILURE);
	}
	Print("Thread %d: got=%s\n", rank, receiveString);
	if (strlen(receiveString) == 0) {
		printf("Nothing\n");
		exit(EXIT_FAILURE);
	}
	
	close(clientSocket);
	sem_post(connectionSemaphore);
	pthread_exit(INT2VOIDP(EXIT_SUCCESS));
}

void realMain(int p, int c) {
	port = p; count = c;
	
	pthread_t* threads = malloc(CLIENT_REQUESTS * sizeof(pthread_t));
	createSemaphore(&connectionSemaphore, connectionSemaphoreName);
	
	double start = 0, end = 0;
	GET_TIME(start);
	for (int i = 0; i < CLIENT_REQUESTS; ++i) {
		pthread_create(&threads[i], NULL, connectToServer, INT2VOIDP(i));
	}
	for (int i = 0; i < CLIENT_REQUESTS; ++i) {
		pthread_join(threads[i], NULL);
	}
	GET_TIME(end);
	printf("%f\n", end - start);
	
	destroySemaphore(connectionSemaphore, connectionSemaphoreName);
	free(threads);
}
