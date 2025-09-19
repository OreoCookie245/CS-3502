#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <time.h>

int main(int argc, char *argv[]){
	int max_lines = -1;
	int verbose = 0;
	int opt;

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
	}


	//Print stats to stderr
	fprintf(stderr, "Lines: %d\n", line_count);
	fprintf(stderr, "Characters: %zu\n", character_count);

	free(line);

	return 0;
}
