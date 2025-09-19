#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

int main(int argc, char *argv[]){
	FILE *input = stdin;
	int buffer_size = 4096;
	char opt;
	char *filename = NULL;
	int file_open = 0;
	char *buffer;

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

	//Read input and write to stdout
	size_t bytes_read;
	while((bytes_read = fread(buffer, 1, buffer_size, input))>0){
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

	//Cleanup
	free(buffer);
	if (file_open == 1){
		fclose(input);
	}

	return 0;
}
