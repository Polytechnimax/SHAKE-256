//
//  Collisions.c
//  SHAKE-256
//
//  Created by LARCHER Maxime on 13/01/2017.
//  Copyright © 2017 LARCHER Maxime. All rights reserved.
//

#include "Collisions.h"


// We assume that the seed has at least the right number of bits
void find_collision(char* seed, int nbits) {
	char *T, *H, *Ttemp, *Htemp;
	
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
	
	printf("Starting point:\n");
	print_as_hexa_string(seed, nbits, 0);
	printf("\nH: \n");
	print_as_hexa_string(H, nbits, 0);
	printf("\nT: \n");
	print_as_hexa_string(T, nbits, 0);
	printf("\n");
	
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
		b &= (H[i] == T[i]);
	}
	
	return b;
}
