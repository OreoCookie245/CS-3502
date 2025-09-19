#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>

volatile sig_atomic_t shutdown_flag = 0;
volatile sig_atomic_t stats_flag = 0;

void handle_sigint ( int sig ) {
	shutdown_flag = 1;
}

void handle_sigusr1 ( int sig ) {
	stats_flag  = 1;
}

int main(int argc, char *argv[]){
	FILE *input = stdin;
	int buffer_size = 4096;
	char opt;
	char *filename = NULL;
	int file_open = 0;
	char *buffer;

        struct sigaction sa ;
        sa.sa_handler = handle_sigint;
        sa.sa_handler = handle_sigusr1;
        sigemptyset (&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction (SIGINT, &sa, NULL);
        sigaction (SIGUSR1, &sa, NULL);

	//Command args
	while((opt = getopt(argc, argv, "f:b:")) != -1){
		switch(opt){
			case 'f':
				filename = optarg;
				break;
			case 'b':
				buffer_size = atoi(optarg);
				break;
			default:
				fprintf(stderr, "Usage: %s [-f filename] [-b buffer_size]\n", argv[0]);
				return 1;
		}
	}

	//Open file if -f
	if (filename != NULL){
		input = fopen(filename, "r");
		if (input == NULL){
			perror("Error opening file");
			return 1;
		}
		file_open = 1;
	}

	//Allocate buffer
	buffer = malloc(buffer_size);
	if(buffer == NULL){
		perror("Error with allocated buffer");
		if (file_open == 1){
			fclose(input);
		}
		return 1;
	}

	size_t character_count = 0;
	clock_t start = clock();

	//Read input and write to stdout
	size_t bytes_read;
	while((bytes_read = fread(buffer, 1, buffer_size, input))>0){
                if(shutdown_flag){
                        fprintf(stderr, "SIGINT received, shutting down\n");
                        break;
                }
                if(stats_flag){
                        double elapsed = (double) (clock() - start) / CLOCKS_PER_SEC;
                        fprintf(stderr, "SIGUSR1 recived, getting current stats\n");
                        fprintf(stderr, "Bytes: %zu\n, MB/s %f/s, Latency: %fs\n",  character_count,
 ((character_count/1024/1024)/elapsed), elapsed);
                        stats_flag = 0;
                }

		size_t bytes_written = fwrite(buffer, 1, bytes_read, stdout);
		if (bytes_read != bytes_written){
			perror("Error writing to stdout");
			free(buffer);
			if (file_open == 1){
				fclose(input);
			}
			return 1;
		}
	}

        clock_t stop = clock();
        double total_time = (double) (stop-start) / CLOCKS_PER_SEC;
        //Print stats to stderr
        fprintf(stderr, "\nProducer Preformance Measurments\n");
        fprintf(stderr, "Bytes per sec: %fs\n", (character_count/total_time));
        fprintf(stderr, "MB/s: %fs\n", ((character_count/1024/1024)/total_time));
        fprintf(stderr, "Latency: %fs\n", total_time);


	//Cleanup
	free(buffer);
	if (file_open == 1){
		fclose(input);
	}

	return 0;
}
