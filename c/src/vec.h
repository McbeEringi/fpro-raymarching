#ifndef VECH
#define VECH

#include <math.h>

typedef struct vec{float x,y,z;} vec_t;// 3つのfloatを持つ型 色の表現にも使う
vec_t vec(float x,float y,float z);

vec_t fadd(vec_t a,float b);
vec_t fmul(vec_t a,float b);
vec_t fdiv(vec_t a,float b);
vec_t vadd(vec_t a,vec_t b);
vec_t vsub(vec_t a,vec_t b);
vec_t vmul(vec_t a,vec_t b);
vec_t vdiv(vec_t a,vec_t b);

float clamp(float x,float a,float b);
float mix(float a,float b,float x);
vec_t vmix(vec_t a,vec_t b,float x);
float smoothstep(float a,float b,float x);
float dot(vec_t a,vec_t b);
vec_t cross(vec_t a,vec_t b);
float fract(float a);
vec_t vfract(vec_t a);
vec_t vfloor(vec_t a);
float length(vec_t a);
vec_t normalize(vec_t a);

#endif