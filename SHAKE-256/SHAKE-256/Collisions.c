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
		if ((s)%100 == 0)
			printf("s=%d\n", s);
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
	
	int i;
	for(i=0; i<ROUNDUP8(nbits); i++) {
		if(!(T[i] & (15 << 4)))
			fprintf(a, "0");
		fprintf(a, "%x", 255 & T[i]);
		if(!(T2[i] & (15 << 4)))
			fprintf(b, "0");
		fprintf(b, "%x", 255 & T2[i]);
	}
	
	
	fclose(a);
	fclose(b);
}



void build_lookalike_collisions(char* model, int modelsize, int nbits, int ntests) {
	char **A = malloc(ntests*sizeof(char*)), **B = malloc(ntests*sizeof(char*));
	int *Asize = malloc(ntests*sizeof(int)), *Bsize = malloc(ntests*sizeof(int));
	int n=0;
	
	char *T1, *T2, *seed = malloc(nbits/8);
	int t1size = 0, t2size = 0;
	
	while(n<ntests) {
		//printf("Looping...\n");
		generate_seed(nbits, seed);
		generate_lookalike_collision(model, modelsize, seed, nbits, &T1, &T2, &t1size, &t2size);
		
		if(!is_already_written(A, B, Asize, Bsize, T1, T2, t1size, t2size, n) && !compare_lookalikes(T1, t1size, T2, t2size)) {
			A[n] = T1;
			B[n] = T2;
			Asize[n] = t1size;
			Bsize[n] = t2size;
			
			write_lookalike_collisions(T1, T2, t1size, t2size, nbits, n);
			
			n++;
		} else {
			free(T1);
			free(T2);
			t1size = 0;
			t2size = 0;
		}
	}
}
void generate_seed(int nbits, char* seed) {
	int i;
	srand((int)time(NULL));
	for(i=0; i<nbits/8; i++) {
		seed[i] = rand() % 256;
	}
}
void generate_lookalike_collision(char* model, int modelsize, char* seed, int nbits, char** T1, char **T2, int *t1size, int* t2size) {
	char *T, *H;
	char *Thash, *Hhash;
	int tsize=0, hsize=0;
	rho_collision(model, modelsize, nbits, seed, &T, &tsize);
	rho_collision(model, modelsize, nbits, seed, &H, &hsize);

	while (1) {
		Thash = SHAKE_256(T, 8*tsize, nbits);
		free(T);
		rho_collision(model, modelsize, nbits, Thash, &T, &tsize);
		
		Hhash = SHAKE_256(H, 8*hsize, nbits);
		free(H);
		rho_collision(model, modelsize, nbits, Hhash, &H, &hsize);
		free(Hhash);
		Hhash = SHAKE_256(H, 8*hsize, nbits);
		free(H);
		rho_collision(model, modelsize, nbits, Hhash, &H, &hsize);
		
		if(compare_lookalikes(T, tsize, H, hsize)) {
			
			/*printf("Found the epact: \n");
			printf("tsize=%d\n", tsize);
			printf("hsize=%d\n", hsize);
			
			printf("T: ");
			int i;
			for(i=0; i<tsize; i++) {
				printf("%x ", T[i]);
			}
			printf("\nH: ");
			for(i=0; i<tsize; i++) {
				printf("%x ", H[i]);
			}
			printf("\n");*/
			
			free(Hhash);
			free(Thash);
			break;
		}
				
		free(Hhash);
		free(Thash);
	}
	
	// Find actual collision
	free(H);
	rho_collision(model, modelsize, nbits, seed, &H, &hsize);
	
	//printf("s: %d\n", s);
	
	while(1) {
		//printf("Looping... ");
		Thash = SHAKE_256(T, 8*tsize, nbits);
		Hhash = SHAKE_256(H, 8*hsize, nbits);
		
		if(compare_hashes(Thash, Hhash, nbits)) {
			free(Hhash);
			free(Thash);
			break;
		}
		
		free(T);
		free(H);
		rho_collision(model, modelsize, nbits, Thash, &T, &tsize);
		rho_collision(model, modelsize, nbits, Hhash, &H, &hsize);
		free(Hhash);
		free(Thash);
	}
	
	/*printf("Found a collision: \n");
	printf("tsize=%d\n", tsize);
	printf("hsize=%d\n", hsize);
	printf("T: ");
	int i;
	for(i=0; i<tsize; i++) {
		printf("%x ", T[i]);
	}
	//printf("has hash: ");
	printf("\nH: ");
	for(i=0; i<tsize; i++) {
		printf("%x ", H[i]);
	}
	printf("\n");*/
	
	*T1 = T;
	*T2 = H;
	*t1size = tsize;
	*t2size = hsize;
}
void rho_collision(char* model, int modelsize, int nbits, char* hash, char **lookalike, int* lookalize_size) {
	int c = count_bits_to_1(hash, nbits), i, j;
	*lookalize_size = modelsize + nbits + c;	// There are nbits '\n' and c ' '
	
	*lookalike = malloc(*lookalize_size);
	for(i=0; i<modelsize; i++) {
		(*lookalike)[i] = model[i];
	}
	(*lookalike)[i] = '\n';
	i++;
	for(j=0; j<nbits; j++) {
		if((hash[j/8] >> (j%8)) & 1) {
			(*lookalike)[i] = ' ';
			i++;
		}
		(*lookalike)[i] = '\n';
		i++;
	}
}
int count_bits_to_1(char* hash, int nbits) {
	int i=0, c=0;
	for(i=0; i<nbits; i++){
		if((hash[i/8] >> (i%8)) & 1)
			c++;
	}
	return c;
}
int compare_lookalikes(char* T, int tsize, char* H, int hsize) {
	int i=0, b=(tsize == hsize);
	while (i<tsize && b) {
		b = T[i] == H[i];
		i++;
	}
	return b;
}
int compare_hashes(char* Thash, char *Hhash, int nbits) {
	int i=0, b=1;
	while(i<nbits/8 && b) {
		b = Thash[i] == Hhash[i];
		i++;
	}
	return b;
}
int is_already_written(char** A, char** B, int *Asize, int *Bsize, char* T1, char* T2, int t1size, int t2size, int n) {
	int i, b=0;
	for(i=0; i<n; i++) {
		if((compare_lookalikes(T1, t1size, A[i], Asize[i]) && compare_lookalikes(T2, t2size, B[i], Bsize[i])) || (compare_lookalikes(T1, t1size, B[i], Bsize[i]) && compare_lookalikes(T2, t2size, A[i], Asize[i]))) {
			b = 1;
			break;
		}
	}
	return b;
}
void write_lookalike_collisions(char* T1, char* T2, int t1size, int t2size, int nbits, int n) {
	char a_name[50], b_name[50];
	sprintf(a_name, "collisions-%d/ex-%d.A", nbits, n);
	sprintf(b_name, "collisions-%d/ex-%d.B", nbits, n);
	FILE* a = fopen(a_name, "w");
	FILE* b = fopen(b_name, "w");
	
	if(a == NULL || b == NULL) {
		printf("Unable to write in the files...\n");
		exit(1);
	}
	
	int i;
	for(i=0; i<t1size; i++) {
		fputc(T1[i], a);
	}
	for(i=0; i<t2size; i++) {
		fputc(T2[i], b);
	}
	
	fclose(a);
	fclose(b);
}









