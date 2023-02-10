#ifndef MAINH
#define MAINH

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "vec.h"

FILE *finit(uint16_t n,uint16_t w,uint16_t h);
void fpush(FILE *f,vec_t c[],uint16_t l);

#endif