#include "common.h"

#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>

int socketLimit;
int firstSocket;
char* localhost = "127.0.0.1";

static int smallPacketModeFlag = 1;

extern void realMain(int port, int count);


void fixOpenFileLimit() {
	struct rlimit fileLimit;
	if (getrlimit(RLIMIT_NOFILE, &fileLimit) == -1) {
		perror("getrlimit");
		exit(EXIT_FAILURE);
	}
	fileLimit.rlim_cur = fileLimit.rlim_max;
	setrlimit(RLIMIT_NOFILE, &fileLimit);
	if (getrlimit(RLIMIT_NOFILE, &fileLimit) == -1) {
		perror("getrlimit");
		exit(EXIT_SUCCESS);
	}
	firstSocket = socket(AF_INET, SOCK_STREAM, 0);
	socketLimit = 100;//(int)(fileLimit.rlim_cur - firstSocket - 1) / 2;
	close(firstSocket);
}

void createSemaphore(sem_t** semaphore, char name[]) {
	sem_unlink(name);
	*semaphore = sem_open(name, O_CREAT, 0644, socketLimit);
	if (*semaphore == SEM_FAILED) {
		perror("sem_open");
		exit(EXIT_FAILURE);
	}
}

void destroySemaphore(sem_t* semaphore, char name[]) {
	if (sem_close(semaphore) == -1) {
		perror("sem_close");
		exit(EXIT_FAILURE);
	}
	if (sem_unlink(name) == -1) {
		perror("sem_unlink");
		exit(EXIT_FAILURE);
	}
}


void usage(char* name) {
	printf("OVERVIEW: ECE 420 Lab 2\n");
	printf("\n");
	printf("USAGE: %s <port> <count>\n", name);
	printf("\n");
	printf("ARGUMENTS:\n");
	printf("  <port>:              The port number of the server\n");
	printf("  <count>:             The number of string in theArray\n");
	printf("\n");
	printf("OPTIONS:\n");
	printf("  --help, -h           Display available options\n");
	printf("\n");
}

int main(int argc, char** argv) {
	for (int i = 1; i < argc; ++i) {
		if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
			usage(argv[0]);
			exit(EXIT_SUCCESS);
		}
	}
	
	if (argc != 3) {
		printf("Bad number of arguments.\n\n");
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	
	char* argvEnd = NULL;
	int port = (int)strtol(argv[1], &argvEnd, 10);
	if (*argvEnd != '\0') {
		printf("Non-numeric argument (port).\n\n");
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	
	argvEnd = NULL;
	int count = (int)strtol(argv[2], &argvEnd, 10);
	if (*argvEnd != '\0') {
		printf("Non-numeric argument (count).\n\n");
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	
	fixOpenFileLimit();
	realMain(port, count);
	return EXIT_SUCCESS;
}

void setSmallPacketMode(int socket) {
	setsockopt(socket,
			   IPPROTO_TCP,
			   TCP_NODELAY,
			   &smallPacketModeFlag,
			   sizeof(smallPacketModeFlag));
}

void readBytes(int socket, char* buffer, int count) {
	int remaining = count;
	while (remaining > 0) {
		ssize_t readCount = read(socket, buffer + count - remaining, remaining);
		if (readCount == -1) {
			perror("read()");
			exit(EXIT_FAILURE);
		}
		remaining -= readCount;
	}
}

void writeBytes(int socket, const char* buffer, int count) {
	int remaining = count;
	while (remaining > 0) {
		ssize_t writeCount = write(socket, buffer + count - remaining, remaining);
		if (writeCount == -1) {
			perror("write()");
			exit(EXIT_FAILURE);
		}
		remaining -= writeCount;
	}
}
