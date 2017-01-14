//
//  Collisions.h
//  SHAKE-256
//
//  Created by LARCHER Maxime on 13/01/2017.
//  Copyright © 2017 LARCHER Maxime. All rights reserved.
//

#ifndef Collisions_h
#define Collisions_h

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Steps.h"

#define NUMBER_OF_8BITS_COLLISIONS 20

#endif /* Collisions_h */

void find_collision(char* seed, int nbits, char** A, char** B);
void build_collisions(int nbits, int ntests);

char* clone(char* src, int nbits);
int compare(char* T, char* H, int nbits);
int already_written(char *A[], char *B[], char* T, char* T2, int n, int nbits);
void store8(char *T, char *T2, int n);
void store(char *T, char *T2, int n, int nbits);
