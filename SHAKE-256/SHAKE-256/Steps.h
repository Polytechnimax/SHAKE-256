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

