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
