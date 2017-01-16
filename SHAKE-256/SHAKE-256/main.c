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
#include <time.h>

#include "Collisions.h"
#include "Steps.h"
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
	
	printf(ANSI_COLOR_RED "usage: ./SHAKE256 [-b | -x] [-n <n>] (-f <file> | -s <string>) -d <d>\n  [-b | -x] The message is given in binary of hexadecimal. By default the message is considered readable text.\n  [-n <n>] The size (in bits) of the message.\n  -f <file> | -s <string> The file or the string to hash.\n  -d <d> The expected length (in bits) of the digest.\n\n");
	
	printf("other usage: ./SHAKE256 -g -n <nbits> -c <ncollisions> [-a]\n  -n <nbits> The number of bits expected.\n  -c <ncollisions>  The number of collisions expected.\n  [-a] Advanced mode, generated examples look like each other.\n" ANSI_COLOR_RESET);
}
void parse_args(int argc, char *argv[], char** file, int* d, int* x, int *b, int* n, int* c, int *g, char** str, int *isfile, int *a) {
	int option = 0;
	int f = 0, s = 0;
	*d = 0;
	*x = 0;
	*b = 0;
	*c = 0;
	*g = 0;
	*a = 0;
	*n = -1;
	while ((option = getopt(argc, argv,"f:s:d:n:c:xbga")) != -1) {
		switch (option) {
			case 'f' : *file = malloc(strlen(optarg)); f = 1;
				strcpy(*file, optarg);
				break;
			case 's' : *str = malloc(strlen(optarg)); s = 1;
				strcpy(*str, optarg);
				break;
			case 'd' : *d = atoi(optarg);
				break;
			case 'x' : *x = 1;
				break;
			case 'b' : *b = 1;
				break;
			case 'g' : *g = 1;
				break;
			case 'a' : *a = 1;
				break;
			case 'n' : *n = atoi(optarg);
				break;
			case 'c' : *c = atoi(optarg);
				break;
			default  : print_usage();
				exit(1);
		}
	}
	if((*g && (!*c || !*n)) || (*g && (*b || *x || f || s || *d)) || (f && s) || (*b && *x) || (!*g && !*d) || (*d && !f && !s)) {
		print_usage();
		exit(1);
	}
	
	*isfile = f;
	
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
	
	char buf[3];
	buf[2] = '\0';
	
	int i;
	for(i=0; i<ROUNDUP8(n); i++) {
		buf[0] = fgetc(file);
		buf[1] = fgetc(file);
		M[i] = strtol(buf, NULL, 16);
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

void compute_hash_from_file(int x, int b, int n, int d, FILE *file) {
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
		//printf(ANSI_COLOR_GREEN "Count: %d\n", count);
		//printf("Count: %d\n", count);
		//printf("Message: %s\n", M);
		//print_as_hexa_string(M, n);
		//printf("\n");
	}
	
	//printf(ANSI_COLOR_RESET);
	D = SHAKE_256(M, n, d);
	print_as_hexa_string(D, d, 0);
	printf("\n");
	free(D);
	free(M);
}
void compute_hash_from_string(int x, int b, int n, int d, char* s) {
	char *M, *D;
	char buf[3];
	buf[2] = '\0';
	if(x){
		if(n > ((int)strlen(s) * 4)) {
			printf("String too short for required message length!\n");
			exit(1);
		}
		if(n<0)
			n = (int)strlen(s) * 4;
		M = malloc(ROUNDUP8(n));
		
		int i;
		for(i=0; i<ROUNDUP8(n); i++) {
			buf[0] = s[2*i];
			buf[1] = s[2*i+1];
			M[i] = strtol(buf, NULL, 16);
		}
		
		M[ROUNDUP8(n)-1] &= ~(255 << ((n+7)%8 + 1));
		
	} else if(b){
		if(n>(int)strlen(s)) {
			printf("String too short for required message length!\n");
			exit(1);
		}
		if(n<0)
			n = (int)strlen(s);
		M = malloc(ROUNDUP8(n));
		
		int i;
		
		for(i=0; i<ROUNDUP8(n); i++) {
			M[i] = 0;
		}
		
		for(i=0; i<n; i++) {
			M[i/8] |= ((s[i] - '0') << (i%8));
		}
		
	} else {
		if(n>(int)strlen(s) * 8) {
			printf("String too short for required message length!\n");
			exit(1);
		}
		if(n<0)
			n = (int)strlen(s) * 8;
		M = malloc((int)strlen(s));
		strcpy(M, s);
	}
	
	D = SHAKE_256(M, n, d);
	print_as_hexa_string(D, d, 0);
	printf("\n");
	free(D);
	free(M);
}

int main(int argc, char * argv[]) {
	
	build_lookalike_collisions("maxime.larcher@polytechnique.edu", 32, 24, 1);
	//build_collisions(32, 1);
	
	/*char* test = malloc(5);
	test[0] = 0;
	test[1] = 0;
	test[2] = 0;
	test[3] = 0;
	test[4] = 0;*/
	
	/*char *S = SHAKE_256("maxime.larcher@polytechnique.edu", 32*8, 32), *S2;
	while(1) {
		S2 = SHAKE_256(S, 32, 32);
		free(S);
		S = S2;
	}*/
	
	/*int d, x, n, b, c, g, a, isfile;
	char *f, *s;
	parse_args(argc, argv, &f, &d, &x, &b, &n, &c, &g , &s, &isfile, &a);
	
	if(g) {
		if(a)
			build_lookalike_collisions("maxime.larcher@polytechnique.edu", 32, n, c);
		else
			build_collisions(n, c);
	} else {
		if(isfile) {
			FILE *file = fopen(f, "r");
			
			if(file==NULL) {
				printf(ANSI_COLOR_RED "File \"%s\" not found.\n" ANSI_COLOR_RESET, f);
				exit(2);
			}
			
			compute_hash_from_file(x, b, n, d, file);
			
			fclose(file);
			free(f);
		} else {
			compute_hash_from_string(x, b, n, d, s);
			free(s);
		}
	}*/
	
	
	/*char *T = malloc(2), *T2 = malloc(2);
	T[0] = 0xb1; T[1] = 0xe3;
	T2[0] = 0x69; T2[1] = 0x3a;
	printf("T:");
	print_as_hexa_string(SHAKE_256(T, 16, 16), 16, 0);
	printf("\nT2: ");
	print_as_hexa_string(SHAKE_256(T2, 16, 16), 16, 0);
	printf("\n");*/
	
	return 0;
}





