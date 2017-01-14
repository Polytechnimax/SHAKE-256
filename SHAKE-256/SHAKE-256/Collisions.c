//
//  Collisions.c
//  SHAKE-256
//
//  Created by LARCHER Maxime on 13/01/2017.
//  Copyright © 2017 LARCHER Maxime. All rights reserved.
//

#include "Collisions.h"

// We assume that the seed has at least the right number of bits
void find_collision(char* seed, int nbits, char** A, char **B) {
	char *T, *H, *Ttemp, *T2temp, *Htemp;
	
	T = clone(seed, nbits);
	H = clone(seed, nbits);
	
	int s = 0;
	while(1) {
		Ttemp = SHAKE_256(T, nbits, nbits);
		free(T);
		T = Ttemp;
		
		Htemp = SHAKE_256(H, nbits, nbits);
		free(H);
		H = SHAKE_256(Htemp, nbits, nbits);
		free(Htemp);
		
		s++;
		
		if(compare(H, T, nbits))
			break;
		
	}
	
	int i;
	char* T2 = clone(seed, nbits);
	for(i=0; i<s-1; i++) {
		Ttemp = SHAKE_256(T, nbits, nbits);
		T2temp = SHAKE_256(T2, nbits, nbits);

		if(compare(Ttemp, T2temp, nbits)) {
			free(Ttemp);
			free(T2temp);
			break;
		}
		
		free(T);
		free(T2);
		T = Ttemp;
		T2 = T2temp;
	}
	
	*A = T;
	*B = T2;
	
	/*printf("Seed: ");
	print_as_hexa_string(seed, nbits, 0);
	
	printf("\nT: ");
	print_as_hexa_string(T, nbits, 0);
	printf("\nSHAKE-256(T): ");
	Ttemp = SHAKE_256(T, nbits, nbits),
	print_as_hexa_string(Ttemp, nbits, 0);
	free(Ttemp);
	
	printf("\nT2: ");
	print_as_hexa_string(T2, nbits, 0);
	printf("\nSHAKE-256(T2): ");
	T2temp = SHAKE_256(T2, nbits, nbits),
	print_as_hexa_string(T2temp, nbits, 0);
	free(T2temp);
	printf("\n");*/
	
}
void build_collisions(int nbits, int ntests) {
	srand((int)time(NULL));
	
	char **A = malloc(ntests*sizeof(char*)), **B = malloc(ntests*sizeof(char*));
	int i, n=0;
	
	char *seed = malloc(ROUNDUP8(nbits));
	char *T1, *T2;
	while (n<ntests) {
		for(i=0; i<ROUNDUP8(nbits); i++) {
			seed[i] = rand() % 255;
		}
		
		find_collision(seed, nbits, &T1, &T2);
		if(!already_written(A, B, T1, T2, n, nbits) && !(compare(T1, T2, nbits))) {
			A[n] = T1;
			B[n] = T2;
			
			store(T1, T2, n, nbits);
			
			n++;
		} else {
			free(T1);
			free(T2);
		}
	}
	
	for(i=0; i<ntests; i++) {
		free(A[i]);
		free(B[i]);
	}
	free(A);
	free(B);
	
}

char* clone(char* src, int nbits) {
	char* dst = malloc(ROUNDUP8(nbits));
	int i;
	
	for(i=0; i<ROUNDUP8(nbits); i++) {
		dst[i] = src[i];
	}
	
	dst[ROUNDUP8(nbits)] &= ~(255 << ((nbits+7)%8 + 1));
	
	return dst;
}
int compare(char* H, char* T, int nbits) {
	int i, b = 1;
	for(i=0; i<ROUNDUP8(nbits); i++) {
		b = b && (H[i] == T[i]);
	}
	
	return b;
}
int already_written(char *A[], char *B[], char* T, char* T2, int n, int nbits) {
	int i, b=0;
	for(i=0; i<n; i++) {
		if((compare(A[i], T, nbits) && compare(B[i], T2, nbits)) || (compare(B[i], T, nbits) && compare(A[i], T2, nbits))) {
			b=1;
			break;
		}
	}
	return b;
}
void store(char *T, char *T2, int n, int nbits) {
	char a_name[50], b_name[50];
	sprintf(a_name, "collisions-%d/ex-%d.A", nbits, n);
	sprintf(b_name, "collisions-%d/ex-%d.B", nbits, n);
	FILE* a = fopen(a_name, "w");
	FILE* b = fopen(b_name, "w");
	
	if(a == NULL || b == NULL) {
		printf("Unable to write in the files...\n");
		exit(1);
	}
	
	fprintf(a, "%x", 255 & T[0]);
	fprintf(b, "%x", 255 & T2[0]);
	
	fclose(a);
	fclose(b);
}
















