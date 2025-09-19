#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main() {
	int pipe1[2];
    	int pipe2[2];
    	pid_t pid;

    	//Create both pipes
    	if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        	perror("Pipe creation failed");
        	return 1;
    	}

    	//Fork
	pid = fork();
	if (pid < 0) {
      		perror("Fork failed");
      		return 1;
   	}

	if (pid == 0) {
	     	close(pipe1[1]);
	        close(pipe2[0]);
		char buffer[BUFFER_SIZE];
		ssize_t bytes;

		while ((bytes = read(pipe1[0], buffer, BUFFER_SIZE - 1)) > 0){
			buffer[bytes] = '\0';
			printf("Child recived: %s", buffer);
			for (int i =0; i < bytes; i++){
				if (buffer[i] >= 'a' && buffer[i] <= 'z') {
					buffer[i] -= 32;
				}
			}
			write(pipe2[1], buffer, bytes);
		}
		close(pipe1[0]);
		close(pipe2[1]);
		return 0;
	}
	else {
		close(pipe1[0]);
                close(pipe2[1]);
		char input[BUFFER_SIZE];
		char response[BUFFER_SIZE];
		ssize_t bytes;

		while (1){
			printf("Enter parent message (type 'stop' to quit): ");
			if(!fgets(input, BUFFER_SIZE, stdin)){
				break;
			}
			if(strncmp(input, "stop", 4) == 0){
				break;
			}
			write(pipe1[1], input, strlen(input));
			bytes = read(pipe2[0], response, BUFFER_SIZE -1);
			if (bytes > 0){
				response[bytes] = '\0';
				printf("Child response to parent: %s", response);
			}
		}
		close(pipe1[1]);
                close(pipe2[0]);
		wait(NULL);
	}
	return 0;
}

