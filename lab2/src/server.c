#include "cast.h"
#include "common.h"
#include "debug.h"
#include "defines.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <setjmp.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <unistd.h>

static char stringInitialFormat[] = "String %d: the initial value";
static char stringWriteFormat[] = "String %d has been modified by a write request";

static char connectionSemaphoreName[] = "/420.server.connection";
static sem_t* connectionSemaphore;

static char** theArray;
static pthread_mutex_t theArrayMutex;

static sigjmp_buf jumpBuffer;

int socketLimit;

void catchSIGINT(int signo) {
	siglongjmp(jumpBuffer, JMP_VALUE_JUMP);
}

void registerSIGINT() {
	struct sigaction sigintNewAction, sigintOldAction;
	sigintNewAction.sa_handler = catchSIGINT;
	sigemptyset(&sigintNewAction.sa_mask);
	sigintNewAction.sa_flags = 0;
	sigaction(SIGINT, &sigintNewAction, &sigintOldAction);
}

int count = 0;

void *HandleClient(void *args) {
	int client = VOIDP2INT(args);
	
	char readString[REQUEST_LENGTH];
	read(client, readString, REQUEST_LENGTH);
	
	bool writeRequest = readString[0] == 'w';
	char* readStringEnd = NULL;
	int index = (int)strtol(readString+1, &readStringEnd, 10);
	
	pthread_mutex_lock(&theArrayMutex);
	
	if (writeRequest) {
		int length = 1 + snprintf(NULL, 0, stringWriteFormat, index);
		theArray[index] = realloc(theArray[index], length * sizeof(char));
		snprintf(theArray[index], length, stringWriteFormat, index);
	}
	write(client, theArray[index], RESPONSE_LENGTH);
	++count;
	Print("Client %d | %d: index=%d, write=%d\n", client, count, index, writeRequest);
	
	pthread_mutex_unlock(&theArrayMutex);
	
	close(client);
	sem_post(connectionSemaphore);
	pthread_exit(INT2VOIDP(EXIT_SUCCESS));
}

void realMain(int port, int count) {
	registerSIGINT();
	
	pthread_t threads[socketLimit];
	createSemaphore(&connectionSemaphore, connectionSemaphoreName);
	pthread_mutex_init(&theArrayMutex, NULL);
	
	theArray = malloc(count * sizeof(char*));
	for (int i = 0; i < count; ++i) {
		int length = 1 + snprintf(NULL, 0, stringInitialFormat, i);
		theArray[i] = malloc(length * sizeof(char));
		snprintf(theArray[i], length, stringInitialFormat, i);
	}
	
	struct sockaddr_in sock_var;
	sock_var.sin_addr.s_addr = inet_addr(localhost);
	sock_var.sin_port = port;
	sock_var.sin_family = AF_INET;
	
	
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (bind(serverSocket, (struct sockaddr*)&sock_var, sizeof(sock_var)) == -1) {
		perror("bind()");
		exit(EXIT_FAILURE);
	}
	
	listen(serverSocket, 2000);
	int jumpResult = sigsetjmp(jumpBuffer, 1);
	while (jumpResult == JMP_VALUE_SET) {
		sem_wait(connectionSemaphore);
		int client = accept(serverSocket, NULL, NULL);
		if (client == -1) {
			Print("accept error: %d\n", errno);
			exit(EXIT_FAILURE);
		}
		pthread_create(&threads[client], NULL, HandleClient, INT2VOIDP(client));
	}
	pthread_mutex_destroy(&theArrayMutex);
	close(serverSocket);
	Print("Server closed\n");
}
