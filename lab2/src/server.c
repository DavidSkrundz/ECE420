#include "cast.h"
#include "common.h"
#include "debug.h"
#include "defines.h"
#include "rwlock.h"

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

//#define FULL_MUTEX 1
//#define PER_STR_MUTEX 1
//#define FULL_RWLOCK 1
#define PER_STR_RWLOCK 1

#ifdef FULL_MUTEX
	static pthread_mutex_t theArrayMutex;
#endif
#ifdef PER_STR_MUTEX
	static pthread_mutex_t* theArrayMutexes;
#endif
#ifdef FULL_RWLOCK
	static rwlock_t theArrayLock;
#endif
#ifdef PER_STR_RWLOCK
	static rwlock_t* theArrayLocks;
#endif

static sigjmp_buf jumpBuffer;

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
	readBytes(client, readString, REQUEST_LENGTH);
	
	bool writeRequest = readString[0] == 'w';
	char* readStringEnd = NULL;
	int index = (int)strtol(readString+1, &readStringEnd, 10);
	
	#ifdef FULL_MUTEX
		pthread_mutex_lock(&theArrayMutex);
	#endif
	#ifdef PER_STR_MUTEX
		pthread_mutex_lock(&theArrayMutexes[index]);
	#endif
	
	if (writeRequest) {
		int length = 1 + snprintf(NULL, 0, stringWriteFormat, index);
		#ifdef FULL_RWLOCK
			rwlock_wlock(&theArrayLock);
		#endif
		#ifdef PER_STR_RWLOCK
			rwlock_wlock(&theArrayLocks[index]);
		#endif
		free(theArray[index]);
		theArray[index] = malloc(length * sizeof(char));
		snprintf(theArray[index], length, stringWriteFormat, index);
		#ifdef FULL_RWLOCK
			rwlock_unlock(&theArrayLock);
		#endif
		#ifdef PER_STR_RWLOCK
			rwlock_unlock(&theArrayLocks[index]);
		#endif
	}
	#ifdef FULL_RWLOCK
		rwlock_rlock(&theArrayLock);
	#endif
	#ifdef PER_STR_RWLOCK
		rwlock_rlock(&theArrayLocks[index]);
	#endif
	writeBytes(client, theArray[index], RESPONSE_LENGTH);
	++count;
	Print("Client %d | %d: index=%d, write=%d\n", client, count, index, writeRequest);
	
	#ifdef FULL_MUTEX
		pthread_mutex_unlock(&theArrayMutex);
	#endif
	#ifdef PER_STR_MUTEX
		pthread_mutex_unlock(&theArrayMutexes[index]);
	#endif
	#ifdef FULL_RWLOCK
		rwlock_unlock(&theArrayLock);
	#endif
	#ifdef PER_STR_RWLOCK
		rwlock_unlock(&theArrayLocks[index]);
	#endif
	
	close(client);
	sem_post(connectionSemaphore);
	pthread_exit(INT2VOIDP(EXIT_SUCCESS));
}

void realMain(int port, int count) {
	registerSIGINT();
	
	pthread_t* threads = malloc(socketLimit * sizeof(pthread_t));
	createSemaphore(&connectionSemaphore, connectionSemaphoreName);
	
	#ifdef FULL_MUTEX
		pthread_mutex_init(&theArrayMutex, NULL);
	#endif
	#ifdef PER_STR_MUTEX
		theArrayMutexes = malloc(count * sizeof(pthread_mutex_t));
		for (int i = 0; i < count; ++i) {
			pthread_mutex_init(&theArrayMutexes[i], NULL);
		}
	#endif
	#ifdef FULL_RWLOCK
		rwlock_init(&theArrayLock);
	#endif
	#ifdef PER_STR_RWLOCK
		theArrayLocks = malloc(count * sizeof(rwlock_t));
		for (int i = 0; i < count; ++i) {
			rwlock_init(&theArrayLocks[i]);
		}
	#endif
	
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
	
	listen(serverSocket, 10000);
	int jumpResult = sigsetjmp(jumpBuffer, 1);
	while (jumpResult == JMP_VALUE_SET) {
		for (int i = 0; i < socketLimit; ++i) {
			sem_wait(connectionSemaphore);
			int client = accept(serverSocket, NULL, NULL);
			if (client == -1) {
				perror("accept()");
				exit(EXIT_FAILURE);
			}
			setSmallPacketMode(client);
			if (pthread_create(&threads[i], NULL, HandleClient, INT2VOIDP(client))) {
				printf("%d\n", i);
				perror("pthread_create()");
				exit(EXIT_FAILURE);
			}
		}
		for (int i = 0; i < socketLimit; ++i) {
			pthread_join(threads[i], NULL);
		}
	}
	#ifdef FULL_MUTEX
		pthread_mutex_destroy(&theArrayMutex);
	#endif
	#ifdef PER_STR_MUTEX
		for (int i = 0; i < count; ++i) {
			pthread_mutex_destroy(&theArrayMutexes[i]);
		}
		free(theArrayMutexes);
	#endif
	#ifdef FULL_RWLOCK
	#endif
	#ifdef PER_STR_RWLOCK
		free(theArrayLocks);
	#endif
	close(serverSocket);
	free(threads);
	Print("Server closed\n");
}
