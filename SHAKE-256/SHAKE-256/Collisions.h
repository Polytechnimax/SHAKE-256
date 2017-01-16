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

void find_collision_advanced(char* lookalike, char* seed, int nbits, char** A, char** B);
void build_collisions_advanced(char* lookalike, int nbits, int ntests);

char* r(char* lookalike, int nbits, char* h, int* out_nbits);
char* r_inv(char* lookalike, char* fake, int nbits);
int count_act_bits(char* h, int nbits);
int compare_advanced(char* lookalike, int nbits, char *T1, char *T2, int t1size, int t2size);
int already_written_advanced(char* lookalike, char** A, char** B, char* T1, char *T2, int n, int nbits);
void store_advanced(char *T, char *T2, int n, int nbits);





void build_lookalike_collisions(char* model, int modelsize, int nbits, int ntests);
void generate_lookalike_collision(char* model, int modelsize, char* seed, int nbits, char** T1, char **T2, int *t1size, int* t2size);
void generate_seed(int nbits, char* seed);
void rho_collision(char* model, int modelsize, int nbits, char* hash, char **lookalike, int* lookalize_size);
int count_bits_to_1(char* hash, int nbits);
int compare_lookalikes(char* T, int tsize, char* H, int hsize);
int compare_hashes(char* Thash, char *Hhash, int nbits);
int is_already_written(char** A, char** B, int *Asize, int *Bsize, char* T1, char* T2, int t1size, int t2size, int n);
void write_lookalike_collisions(char* T1, char* T2, int t1size, int t2size, int nbits, int n);






