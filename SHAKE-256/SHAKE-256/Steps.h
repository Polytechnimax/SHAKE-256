//
//  Steps.h
//  SHAKE-256
//
//  Created by LARCHER Maxime on 06/01/2017.
//  Copyright © 2017 LARCHER Maxime. All rights reserved.
//

#ifndef Steps_h
#define Steps_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define CHARSIZE (sizeof(char) * 8)
#define ALIGNON8(w) ((5*w+7) & (~7))
#define UPPER8MULTIPLE(x) ((x+7) & (~7))
#define ROUNDUP8(x) ((x+7) >> 3)

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

#endif /* Steps_h */

char* theta(char* S, int w);
char* rho(char* A, int w);
char* pi(char* A, int w);
char* chi(char* A, int w);
char* iota(char* A, int ir, int w);

char* Rnd(char* S, int ir, int w);
char* Keccac_p(int b, int nr, char* S);
char* SHAKE_256(char* N, int nbits, int d);

char* pad_message(char* N, int nbits, int *n);
char* pad_with_0(char* P);
char* XOR(char* S, char* Pad);
char* all_zero();
char* construct_digest(char* Z, char* S, int* zbits);
char* trunc_at(char* Z, int d);

void print_as_string(char* S, int w);
void print_as_array(char* S, int w);
void compare_as_array(char* S, char* Sprime, int w);
// Only works if w>=8
void print_as_sheets(char* S, int w);
void print_as_hexa_string(char* S, int nbits, int space);

