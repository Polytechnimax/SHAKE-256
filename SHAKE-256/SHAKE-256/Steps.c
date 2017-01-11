//
//  Steps.c
//  SHAKE-256
//
//  Created by LARCHER Maxime on 06/01/2017.
//  Copyright © 2017 LARCHER Maxime. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Steps.h"

#define CHARSIZE (sizeof(char) * 8)
#define ALIGNON8(w) ((5*w+7) & (~7))
#define UPPER8MULTIPLE(x) ((x+7) & (~7))
#define ROUNDUP8(x) ((x+7) >> 3)
/*
 * How does this code work?
 * Instead of transforming the string S in an array we work directly on the string. The following macros help 
 * access the elements as if it was a state array.
 */

#define GET_STRING_BITINDEX(x,y,z,w) (w*(5*y+x) + z)
#define GET_STRING_CHARINDEX(x,y,z,w) (GET_STRING_BITINDEX(x,y,z,w) / CHARSIZE)
#define GET_STRING_BITINDEXINCHAR(x,y,z,w) ((GET_STRING_BITINDEX(x,y,z,w) % CHARSIZE))
//#define GET_STRING_BITINDEXINCHAR(x,y,z,w) (7 - (GET_STRING_BITINDEX(x,y,z,w) % CHARSIZE))
#define GET_ARRAY_ELEMENTAT(S,x,y,z,w) ((S[GET_STRING_CHARINDEX(x,y,z,w)] >> (GET_STRING_BITINDEXINCHAR(x,y,z,w))) & 1)

#define SET_ARRAY_ELEMENTAT(S,x,y,z,w,b) (S[GET_STRING_CHARINDEX(x,y,z,w)] \
										= ((~b+1) & (S[GET_STRING_CHARINDEX(x,y,z,w)] | (1 << GET_STRING_BITINDEXINCHAR(x,y,z,w)))) \
										| ((~(~b+1)) & (S[GET_STRING_CHARINDEX(x,y,z,w)] & ~(1 << GET_STRING_BITINDEXINCHAR(x,y,z,w)))))

#define GET_THETATEMP_BITAT(C,x,z) ((C[(w*x + z) / 8] >> ((w*x + z) % 8)) & 1)
#define SET_THETATEMP_BITAT(C,x,z,b) (C[(w*x + z) / 8] \
										= ((~b+1) & ((C[(w*x + z) / 8]) | (1 << ((w*x + z) % 8)))) \
										| ((~(~b+1)) & ((C[(w*x + z) / 8] & (~(1 << ((w*x + z) % 8)))))))

/*
 * Theta, rho, pi, chi and iota are the steps used in Keccac_p
 * Each returns a new string but does not free the one given in argument
 * Since it is impossible to guess the size of a string in C, the size must be specified (w = b/25).
 */
char* theta(char* S, int w) {
	
	char *C,*D,*Sprime;
	C = malloc(ROUNDUP8(5*w));
	D = malloc(ROUNDUP8(5*w));
	Sprime = malloc(ROUNDUP8(25*w));
	
	int i, j, k;
	
	int C1[5][64], D1[5][64];
	
	
	// Fill C
	for(i=0; i<5; i++) {
		for(k=0; k<w; k++) {
			char c = 0;
			for (j=0; j<5; j++) {
				c ^= GET_ARRAY_ELEMENTAT(S, i, j, k, w);
			}
			SET_THETATEMP_BITAT(C, i, k, c);
			C1[i][k] = c;
		}
	}
	
	// Fill D
	for(i=0; i<5; i++) {
		for(k=0; k<w; k++) {
			char c = GET_THETATEMP_BITAT(C, (i+4)%5, k) ^ GET_THETATEMP_BITAT(C, (i+1) % 5, (k+w-1)%w);
			D1[i][k] = C1[(i+4)%5][k] ^ C1[(i+1)%5][(k-1+w)%w];
			SET_THETATEMP_BITAT(D, i, k, c);
		}
	}
	
	// Modify S
	for(i=0; i<5; i++) {
		for(j=0; j<5; j++) {
			for(k=0; k<w; k++) {
				//char b = GET_THETATEMP_BITAT(D, i, k) ^ GET_ARRAY_ELEMENTAT(S, i, j, k, w);
				char b = D1[i][k] ^ GET_ARRAY_ELEMENTAT(S, i, j, k, w);
				SET_ARRAY_ELEMENTAT(Sprime, i, j, k, w, b);
			}
		}
	}
	
	free(C);
	free(D);
	
	return Sprime;
	
}
char* rho(char* S, int w){
	char* Sprime = malloc(ROUNDUP8(25*w));
	
	int i=1,j=0,k;
	for(k=0;k<w; k++) {
		SET_ARRAY_ELEMENTAT(Sprime, 0, 0, k, w, GET_ARRAY_ELEMENTAT(S, 0, 0, k, w));
	}
	
	int t;
	for(t=0;t<24;t++) {
		for(k=0;k<w;k++) {
			int accessedk = (k-(t+1)*(t+2)/2)%w;
			while (accessedk < 0) {
				accessedk += w;
			}
			accessedk %= w;
			char b = GET_ARRAY_ELEMENTAT(S, i, j, accessedk, w);
			SET_ARRAY_ELEMENTAT(Sprime, i, j, k, w, b);
		}
		int temp = j;
		j = (2*i+3*j)%5;
		i = temp;
	}
	
	return Sprime;
}
char* pi(char* S, int w) {
	char* Sprime = malloc(ROUNDUP8(25*w));
	
	int i,j,k;
	for(i=0;i<5;i++) {
		for(j=0;j<5;j++){
			for(k=0;k<w;k++){
				SET_ARRAY_ELEMENTAT(Sprime, i, j, k, w, GET_ARRAY_ELEMENTAT(S, ((i+3*j)%5), i, k, w));
			}
		}
	}
	return Sprime;
}
char* chi(char* S, int w) {
	char* Sprime = malloc(ROUNDUP8(25*w));
	
	int i,j,k;
	for(i=0;i<5;i++) {
		for(j=0;j<5;j++){
			for(k=0;k<w;k++){
				char b = GET_ARRAY_ELEMENTAT(S, i, j, k, w) ^ ((GET_ARRAY_ELEMENTAT(S, (i+1)%5, j, k, w) ^ 1) & (GET_ARRAY_ELEMENTAT(S, (i+2)%5, j, k, w)));
				SET_ARRAY_ELEMENTAT(Sprime, i, j, k, w, b);
			}
		}
	}
	return Sprime;
}
int rc(int t) {
	int r=1, i, teff = t%255;
	for(i=0; i<teff; i++) {
		char r8 = (char)((r >> 7) & 1);
		r = r << 1;
		int mask = r8 + (r8 << 4) + (r8 << 5) + (r8 << 6);
		r ^= mask;
		r &= 255;
	}
	return r & 1;
}
char* iota(char* S, int ir, int w) {
	char* Sprime = malloc(ROUNDUP8(25*w));
	
	int i,j,k;
	for(i=0; i<5; i++){
		for(j=0; j<5; j++){
			for(k=0;k<w;k++) {
				SET_ARRAY_ELEMENTAT(Sprime, i, j, k, w, GET_ARRAY_ELEMENTAT(S, i, j, k, w));
			}
		}
	}
	
	unsigned long RC = 0;
	
	// TODO: Check if this is striclty inferior or not
	for(j=0; 1 << j <= w; j++) {
		RC |= (((unsigned long)rc(j+7*ir)) << ((1 << j) - 1));
	}
	
	//printf("RC=%lx\n", RC);
	
	for(k=0; k<w; k++) {
		SET_ARRAY_ELEMENTAT(Sprime, 0, 0, k, w, (GET_ARRAY_ELEMENTAT(Sprime, 0, 0, k, w) ^ ((RC >> k) & 1)));
	}
	
	return Sprime;
}

/*
 * Rnd is the composition of the previous function. It does not free the string in argument.
 */
char* Rnd(char* S, int ir, int w) {
	char* S1 = theta(S, w);
	//printf("After theta: \n");
	//print_as_sheets(S1, w);
	
	char* S2 = rho(S1, w);
	//printf("After rho: \n");
	//print_as_sheets(S2, w);

	char* S3 = pi(S2, w);
	//printf("After pi: \n");
	//print_as_sheets(S3, w);

	char* S4 = chi(S3, w);
	//printf("After chi: \n");
	//print_as_sheets(S4, w);

	char* S5 = iota(S4, ir, w);
	//printf("After iota: \n");
	//print_as_sheets(S5, w);
	//printf("\n");

	free(S1);
	free(S2);
	free(S3);
	free(S4);
	
	return S5;
}

char* Keccac_p(int b, int nr, char* S) {
	int l, w=b/25;
	for(l=0;1<<l < w; l++) {
		
	}
	
	char *S1, *S2;
	S1 = S;
	
	int ir, i=0;
	for(ir=12+2*l - nr; ir<12+2*l; ir++) {
		
		//printf("--- Round %d ---\n\n", i);
		S2 = Rnd(S1, ir, w);
		//free(S1);
		S1 = S2;
		i++;
	}
	
	return S1;
}
char* SHAKE_256(char* N, int nbits, int d) {
	int b=1600;
	int n = 0;

	char* Pad, *Xor;
	char* P = pad_message(N, nbits, &n);
	char* S = all_zero();
	
	n /= 1088;
	
	int i;
	for(i=0; i<n; i++) {
		Pad = pad_with_0(P + (i*1088/8));
		Xor = XOR(S, Pad);
		free(Pad);
		free(S);
		//print_as_hexa_string(Xor, b);
		S = Keccac_p(b, 24, Xor);
		free(Xor);
	}
	
	char *Z, *Ztemp, *Stemp;
	int zbits = 0;
	
	Z = malloc(1);
	
	i=0;
	while (zbits < d) {
		//printf("--- Round %d ---\n", i);
		//print_as_hexa_string(S, b);
		//printf("\n\n");
		Ztemp = construct_digest(Z, S, &zbits);
		free(Z);
		Z = Ztemp;
		
		Stemp = S;
		S = Keccac_p(b, 24, Stemp);
		free(Stemp);
		i++;
	}
	
	Ztemp = trunc_at(Z, d);
	free(Z);
	Z = Ztemp;
	
	return Z;
}

char* pad_message(char* N, int nbits, int *n) {
	int r = 1088;
	
	// We have an extra -4 because SHAKE-256 requires 1111 added at the end
	int j = (-nbits-6) % r;
	while (j<0) 
		j+=r;
	j%=r;
	
	int tot_bits = nbits+j+6;
	
	char* P = malloc(tot_bits/8);
	
	int i;
	for(i=0; i<ROUNDUP8(nbits);i++) {
		P[i] = N[i];
	}
	for(i=ROUNDUP8(nbits); i<tot_bits/8; i++) {
		P[i] = 0;
	}
	P[ROUNDUP8(nbits)-1] &= ~(255 << (((nbits+7)%8) + 1));
	P[tot_bits/8 - ROUNDUP8(j+6)] |= 1 << (7-(j+5)%8);
	P[tot_bits/8 - ROUNDUP8(j+5)] |= 1 << (7-(j+4)%8);
	P[tot_bits/8 - ROUNDUP8(j+4)] |= 1 << (7-(j+3)%8);
	P[tot_bits/8 - ROUNDUP8(j+3)] |= 1 << (7-(j+2)%8);
	P[tot_bits/8 - ROUNDUP8(j+2)] |= 1 << (7-(j+1)%8);
	P[tot_bits/8 - 1] |= 1 << 7;
	
	*n = tot_bits;

	return P;
}
char* pad_with_0(char* P) {
	// In this case we know b = 1600, c = 512, r = 1088
	char* Pad = malloc(1600/8);
	int i;
	for(i=0; i<(1088/8); i++) {
		Pad[i] = P[i];
	}
	for (i=1088/8; i<1600/8; i++) {
		Pad[i] = 0;
	}
	
	return Pad;
	
}
char* XOR(char* S, char* Pad) {
	// In this case we know b = 1600, c = 512, r = 1088
	char* xor = malloc(1600/8);
	int i;
	for(i=0; i<1600/8; i++) {
		xor[i] = S[i] ^ Pad[i];
	}
	
	return xor;
}
char* all_zero() {
	char* S = malloc(1600/8);
	int i;
	for(i=0; i<1600/8; i++) {
		S[i] = 0;
	}
	return S;
}
char* construct_digest(char* Z, char* S, int* zbits) {
	*zbits += 1088;
	char* Z2 = malloc(*zbits);
	
	int i;
	for(i=0; i<*zbits-1088; i++) {
		Z2[i] = Z[i];
	}
	for(i=0; i<1088; i++) {
		Z2[*zbits-1088+i] = S[i];
	}
	
	return Z2;
}
char* trunc_at(char* Z, int d) {
	char* Trunc = malloc(ROUNDUP8(d));
	int i;
	for(i=0; i<ROUNDUP8(d); i++) {
		Trunc[i] = Z[i];
	}
	
	Trunc[ROUNDUP8(d) - 1] &= 255 << ((8-(d%8))%8);
	
	return Trunc;
}

void print_as_string(char* S, int w) {
	
	int i;
	for(i=0; i<25*w; i++) {
		printf("%d", (S[i/8] >> (7 - (i%8))) & 1);
		if((i-7)%8 == 0) {
			printf(" ");
		}
	}
	printf("\n");
}
void print_as_array(char* S, int w) {
	
	int i,j,k;
	for(i=0;i<5;i++) {
		printf("Sheet %d\n", i);
		for(j=0;j<5;j++) {
			for(k=0;k<w;k++) {
				printf("%d", GET_ARRAY_ELEMENTAT(S, i, j, k, w));
			}
			printf("\n");
		}
		printf("\n");
	}
}
void compare_as_array(char* S, char* Sprime, int w) {
	int i,j,k;
	for(i=0;i<5;i++) {
		for(j=0;j<5;j++) {
			for(k=0;k<w; k++) {
				char b = GET_ARRAY_ELEMENTAT(S, i, j, k, w), bprime = GET_ARRAY_ELEMENTAT(Sprime, i, j, k, w);
				if(b == bprime)
					printf("*");
				else
					printf("%d", bprime);
			}
			printf("\n");
		}
		printf("\n");
	}
}

void print_as_sheets(char* S, int w) {
	int i,j,k;
	for(i=0;i<5;i++) {
		for(j=0;j<5;j++) {
			for(k=(5*i+j)*w/8 + 7; k>=(5*i+j)*w/8; k--) {
				if(!(S[k] & (15 << 4)))
					printf("0");
				printf("%x", (S[k] & 255));
			}
			printf(" ");
		}
		printf("\n");
	}
}
void print_as_hexa_string(char* S, int nbits) {
	int i;
	for(i=0; i<ROUNDUP8(nbits); i++) {
		if(!(S[i] & (15 << 4)))
			printf("0");
		printf("%x ", (S[i] & 255));
	}
}












