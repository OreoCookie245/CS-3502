#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <signal.h>

volatile sig_atomic_t shutdown_flag = 0;
volatile sig_atomic_t stats_flag = 0;

void handle_sigint ( int sig ) {
        shutdown_flag = 1;
}

void handle_sigusr1 ( int sig ) {
        stats_flag = 1;
}

int main(int argc, char *argv[]){
	int max_lines = -1;
	int verbose = 0;
	int opt;

        struct sigaction sa ;
        sa.sa_handler = handle_sigint;
        sa.sa_handler = handle_sigusr1;
        sigemptyset (&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction (SIGINT, &sa, NULL);
        sigaction (SIGUSR1, &sa, NULL);

	//Command args
	while((opt = getopt(argc, argv, "n:v")) != -1){
		switch(opt){
			case 'n':
				max_lines = atoi(optarg);
				if (max_lines <= 0){
					fprintf(stderr, "Error: Max lines must be positive\n");
					return 1;
				}
				break;
			case 'v':
				verbose = 1;
				break;
			default:
				fprintf(stderr, "Usage: %s [-n max_lines] [-v verbose]\n", argv[0]);
				return 1;
		}
	}

	char *line = NULL;
	size_t len = 0;
	int line_count = 0;
	size_t read;
	size_t character_count = 0;

	clock_t start = clock();

	//Read stdin
	while((read = getline(&line, &len, stdin)) != -1){
		//Count lines to see if at max
		if(max_lines != -1 && line_count >= max_lines){
			break;
		}
		//Verbose then echo to stdout
		if (verbose){
			fwrite(line, 1, read, stdout);
		}
		//Count chars & lines
                character_count += read;
                line_count++;

		if(shutdown_flag){
                	fprintf(stderr, "SIGINT received, shutting down\n");
                	break;
        	}
        	if(stats_flag){
			double elapsed = (double) (clock() - start) / CLOCKS_PER_SEC;
                	fprintf(stderr, "SIGUSR1 recived, getting current stats\n");
                	fprintf(stderr, "Lines: %d, Bytes: %zu\n, MB/s: %fs\n, Latancy: %fs\n", line_count, 
character_count, ((character_count/1024/1024)/elapsed), elapsed);
        	        stats_flag = 0;
	        }

	}

	clock_t stop = clock();
	double total_time = (double) (stop-start) / CLOCKS_PER_SEC;
	//Print stats to stderr
	fprintf(stderr, "\nConsumer Preformance Measurments\n");
	fprintf(stderr, "Lines per sec: %fs\n", (line_count/total_time));
	fprintf(stderr, "Bytes per sec: %fs\n", (character_count/total_time));
	fprintf(stderr, "MB/s: %fs\n", ((character_count/1024/1024)/total_time));
	fprintf(stderr, "Latency: %fs\n", total_time);


	free(line);

	return 0;
}
