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

void print_usage() {
	printf("Usage: SHAKE256 -f <file> -d <d> [-x] [-n <n>]\n  -f <file> The file containing the string to hash.\n  -d <d> The expected size of the digest.\n  [-x] Specifies if the string is given as a list of hexadecimal values. By default, the string is readable text.\n  [-n <n>] The size (in bits) of the message to hash. Must be specified if the message is given as hexadecimal values or as a \"non-readable\" string.\n");
	
}

void parse_args(int argc, char *argv[], char** file, int* d, int* x, int* n) {
	int option = 0;
	int f = 0;
	*d = 0;
	*x = 0;
	*n = -1;
	while ((option = getopt(argc, argv,"f:d:xn:")) != -1) {
		switch (option) {
			case 'f' : *file = malloc(strlen(optarg)); f = 1;
				strcpy(*file, optarg);
				break;
			case 'd' : *d = atoi(optarg);
				break;
			case 'x' : *x = 1;
				break;
			case 'n' : *n = atoi(optarg);
				break;
			default  : print_usage();
				exit(1);
		}
	}
	
	if(f==0 || *d==0){
		print_usage();
		exit(1);
	}
}

int main(int argc, /*const*/ char * argv[]) {
	
	int d, x, n;
	char* file;
	parse_args(argc, argv, &file, &d, &x, &n);
	
	printf("File: %s\nd: %d\nx: %d\nn: %d\n", file, d, x, n);
	
	return 0;
}





