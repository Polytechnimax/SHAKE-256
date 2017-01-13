//
//  main.c
//  SHAKE-256
//
//  Created by LARCHER Maxime on 06/01/2017.
//  Copyright © 2017 LARCHER Maxime. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "Steps.h"
#include <time.h>
#include "Tests.h"

#define ROUNDUP8(x) ((x+7) >> 3)

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void print_usage() {
	printf(ANSI_COLOR_RED "Usage: SHAKE256 -f <file> -d <d> [-x] [-b] [-n <n>]\n  -f <file> The file containing the string to hash.\n  -d <d> The expected size of the digest.\n  [-x] Specify if the string is given in hexadecimal.\n  [-b] Specify if the string is given in bits.\n  [-n <n>] The size (in bits) of the message to hash.\n" ANSI_COLOR_RESET);
	
}
void parse_args(int argc, char *argv[], char** file, int* d, int* x, int *b, int* n) {
	int option = 0;
	int f = 0;
	*d = 0;
	*x = 0;
	*b = 0;
	*n = -1;
	while ((option = getopt(argc, argv,"f:d:xbn:")) != -1) {
		switch (option) {
			case 'f' : *file = malloc(strlen(optarg)); f = 1;
				strcpy(*file, optarg);
				break;
			case 'd' : *d = atoi(optarg);
				break;
			case 'x' : *x = 1;
				break;
			case 'b' : *b = 1;
				break;
			case 'n' : *n = atoi(optarg);
				break;
			default  : print_usage();
				exit(1);
		}
	}
	
	if(f==0 || *d==0 || (*x && *b)){
		print_usage();
		exit(1);
	}
}
int count_hex(FILE *file) {
	int i=0;
	char c;
	while (1) {
		c = fgetc(file);
		if(c == EOF)
			break;
		if(c != ' ')
			i++;
	}
	rewind(file);
	return i*4;
}
int count_non_hex(FILE *file) {
	int i=0;
	char c;
	while (1) {
		c = fgetc(file);
		if(c == EOF)
			break;
		i++;
	}
	rewind(file);
	return i*8;
}
char* get_hex_msg(FILE* file, int n) {
	char *M = malloc(ROUNDUP8(n));
	
	int c=0, i=0;
	while (i<ROUNDUP8(n)) {
		fscanf(file, "%x", &c);
		M[i] = c;
		i++;
	}
	
	return M;
}
char* get_non_hex_msg(FILE* file, int n) {
	char *M = malloc(ROUNDUP8(n));
	
	int i = 0;
	char c;
	while (1) {
		c = fgetc(file);
		if(i>=ROUNDUP8(n) || c == EOF)
			break;
		M[i] = c;
		i++;
	}
	
	M[ROUNDUP8(n) - 1] &= ~(255 << (((n+7)%8) + 1));
	
	fseek(file, 0, SEEK_SET);
	
	return M;
}
char* get_bin_msg(FILE* file, int n) {
	char* M = malloc(ROUNDUP8(n));
	
	int i;
	
	for(i=0; i<ROUNDUP8(n); i++) {
		M[i] = 0;
	}
	
	i=0;
	char c;
	while (1) {
		c = fgetc(file);
		if(i>=ROUNDUP8(n) || c == EOF)
			break;
		M[i/8] |= (c - '0') << (i%8);
		i++;
	}
	
	rewind(file);
	
	return M;
	
}

void compute_hash(int x, int b, int n, int d, FILE *file) {
	int count;
	char *M, *D;
	if(x) {
		count = count_hex(file);
		if(n>count) {
			printf(ANSI_COLOR_RED "File too short for required message length!\n" ANSI_COLOR_RESET);
			exit(1);
		}
		if(n<0)
			n = count;
		M = get_hex_msg(file, n);
		/*printf(ANSI_COLOR_GREEN "Count: %d\n", count);
		 printf("Message:\n");
		 print_as_hexa_string(M, n);
		 printf("\n");*/
	} else if (b){
		count = count_non_hex(file);
		count /=8;
		if(n>count) {
			printf(ANSI_COLOR_RED "File too short for required message length!\n" ANSI_COLOR_RESET);
			exit(1);
		}
		if(n<0)
			n = count;
		M = get_bin_msg(file, n);
		/*printf(ANSI_COLOR_GREEN "Count: %d\n", count);
		 printf("Message:\n");
		 print_as_hexa_string(M, n);
		 printf("\n");*/
	} else {
		count = count_non_hex(file);
		if(n>count) {
			printf(ANSI_COLOR_RED "File too short for required message length!\n" ANSI_COLOR_RESET);
			exit(1);
		}
		if(n<0)
			n = count;
		M = get_non_hex_msg(file, n);
		/*printf(ANSI_COLOR_GREEN "Count: %d\n", count);
		 printf("Count: %d\n", count);
		 printf("Message:\n");
		 print_as_hexa_string(M, n);
		 printf("\n");*/
	}
	
	printf(ANSI_COLOR_RESET);
	D = SHAKE_256(M, n, d);
	print_as_hexa_string(D, d, 0);
	printf("\n");
	free(D);
	free(M);
}

int main(int argc, char * argv[]) {
	
	int d, x, n, b;
	char* f;
	parse_args(argc, argv, &f, &d, &x, &b, &n);
	
	FILE *file = fopen(f, "r");
	
	if(file==NULL) {
		printf(ANSI_COLOR_RED "File \"%s\" not found.\n" ANSI_COLOR_RESET, f);
		exit(2);
	}
	
	compute_hash(x, b, n, d, file);
	
	fclose(file);
	return 0;
}





