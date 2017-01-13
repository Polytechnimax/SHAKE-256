//
//  Tests.c
//  SHAKE-256
//
//  Created by LARCHER Maxime on 08/01/2017.
//  Copyright © 2017 LARCHER Maxime. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "Tests.h"
#include "Steps.h"

void test_all_zero() {
	char* S = all_zero();
	//S[0] = 1;
	//print_as_sheets(S, 64);
	//printf("\n");
	
	char* S2 = Keccac_p(1600, 24, S);
	
	//free(S);
	free(S2);
	
}

void test_place_ones() {
	char all_zero[200];
	int i;
	for(i=0;i<200;i++) {
		all_zero[i] = 0;
	}
	
	int w = 64;

	printf("Starting point: \n");
	print_as_sheets(all_zero, w);
	
	printf("After changing one bit: \n");
	print_as_sheets(all_zero, w);
}

void print_string(char *M, int nbits) {
	int i;
	for(i=0;i<nbits; i++) {
		printf("%d", (M[i/8] >> (7- (i%8))) & 1);
		if(!((i+1)%8))
			printf(" ");
	}
}

void test_padding() {
	char N[200];
	int i;
	for(i=0; i<200; i++) {
		N[i] = 0;
	}
	int s = 8;
	N[0] = 167;
	
	printf("N:\n");
	print_as_hexa_string(N, s, 0);
	
	int n = 0;
	char* M = pad_message(N, s, &n);
	printf("\n\nM:\n");
	print_as_hexa_string(M, n, 0);
	printf("\n");
}

void test_SHAKE() {
	char N[204];
	int i;
	for(i=0; i<204; i++) {
		N[i] = 0xa3;
	}
	
	char* digest = SHAKE_256(N, 1630, 1024);
	
	//printf("\n\n");
	print_as_hexa_string(digest, 1024, 0);
}
