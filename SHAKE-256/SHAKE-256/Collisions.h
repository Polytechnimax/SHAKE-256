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

#include "Steps.h"

#endif /* Collisions_h */

void find_collision(char* seed, int nbits);
char* clone(char* src, int nbits);
int compare(char* T, char* H, int nbits);
