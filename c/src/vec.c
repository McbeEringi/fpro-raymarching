#include "vec.h"

// vec_tはヘッダーで定義済み

// ラッパー
vec_t vec(float x,float y,float z){vec_t a;a.x=x;a.y=y;a.z=z;return a;}

// 四則演算
vec_t fadd(vec_t a,float b){return vec(a.x+b,a.y+b,a.z+b);}
vec_t fmul(vec_t a,float b){return vec(a.x*b,a.y*b,a.z*b);}
vec_t fdiv(vec_t a,float b){return vec(a.x/b,a.y/b,a.z/b);}
vec_t vadd(vec_t a,vec_t b){return vec(a.x+b.x,a.y+b.y,a.z+b.z);}
vec_t vsub(vec_t a,vec_t b){return vec(a.x-b.x,a.y-b.y,a.z-b.z);}
vec_t vmul(vec_t a,vec_t b){return vec(a.x*b.x,a.y*b.y,a.z*b.z);}
vec_t vdiv(vec_t a,vec_t b){return vec(a.x/b.x,a.y/b.y,a.z/b.z);}

// その他演算
float clamp(float x,float a,float b){return fmin(fmax(x,a),b);}
float smoothstep(float a,float b,float x){float t=clamp((x-a)/(b-a),0.,1.);return t*t*(3.-2.*t);}// エルミート補間
float mix(float a,float b,float x){return a*(1.-x)+b*x;}// 線形補間
vec_t vmix(vec_t a,vec_t b,float x){return vadd(fmul(a,1.-x),fmul(b,x));}
float dot(vec_t a,vec_t b){vec_t c=vmul(a,b);return c.x+c.y+c.z;}// 内積
vec_t cross(vec_t a,vec_t b){return vec(a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x);}// 外積
float fract(float a){return fmod(fmod(a,1.)+1.,1.);}// 小数部分 負でも0~1
vec_t vfract(vec_t a){return vec(fract(a.x),fract(a.y),fract(a.z));}
vec_t vfloor(vec_t a){return vec(floor(a.x),floor(a.y),floor(a.z));}
float length(vec_t a){return sqrt(a.x*a.x+a.y*a.y+a.z*a.z);}// ノルム
vec_t normalize(vec_t a){return fdiv(a,length(a));}// 正規化

