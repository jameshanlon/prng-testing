/*
 * Copyright (c) Stephex Felix 2022.
 */

#include "stdio.h"
#include "stdint.h"
#include "math.h"
#include "float.h"
#define __STDC_FORMAT_MACROS
#include "inttypes.h"

int prngupdate_xoro(int x, int y, int z, uint64_t t[2], uint64_t *tp)
{
// variant used by GC : x,y,z = 55,14,36
// alternative variant: x,y,z = 24,16,37
uint64_t s0, s1;
s0 = t[0];
s1 = t[1];
s1 ^= s0;
s0 = ((s0<<x) | (s0>>(64-x))) ^ s1 ^ (s1<<y);
s1 = ((s1<<z) | (s1>>(64-z)));
*(tp+0) = s0;
*(tp+1) = s1;
return 1;
}

int printvec(uint64_t t[2])
{
int c,b,i;
i=0;
for (c=127;c>=0;c--) {b=(t[(int)(c/64)]>>(c&63))&1;printf("%d",b);i+=b;}
printf("   %d ones\n",i);
return 1;
}

int printvech(uint64_t t[2])
{
int c,b;
for (c=31;c>=0;c--) {b=(t[(int)(c/16)]>>((c*4)%64))&15;printf("%1x",b);}
printf("\n");
return 1;
}

int printvechc(uint64_t t[2])
{
int c,b;
for (c=31;c>=0;c--) {b=(t[(int)(c/16)]>>((c*4)%64))&15;printf("%1x",b);}
printf("   // static const uint64_t JUMP[] = { 0x");
for (c=15;c>=0;c--) {b=(t[(int)(c/16)]>>((c*4)%64))&15;printf("%1x",b);}
printf(", 0x");
for (c=31;c>=16;c--) {b=(t[(int)(c/16)]>>((c*4)%64))&15;printf("%1x",b);}
printf(" };\n");
return 1;
}

int transpose(uint64_t m[128][2], uint64_t *t)
{
int r,c;

for (r=0;r<128;r++) {
  *(t+2*r)=0; *(t+2*r+1)=0;
  for (c=0;c<128;c++) {
    *(t+2*r+(int)(c/64))|=((m[c][(int)(r/64)]>>(r & 63)) & 1) << (c & 63);
  }
}
return 1;
}

int mmult(uint64_t m[128][2], uint64_t n[128][2], uint64_t *t)
{
int r,c,i,j,k,k2;
uint64_t bit[2];
uint64_t nt[128][2];
uint64_t kl;

r = transpose(n, &nt[0][0]);

for (r=0;r<128;r++) {
  *(t+2*r)=0; *(t+2*r+1)=0;
  for (c=0;c<128;c++) {
    bit[0] = m[r][0] & nt[c][0];
    bit[1] = m[r][1] & nt[c][1];
    kl = bit[0] ^ bit[1];
    k  = (unsigned int)(kl>>32);
    k2 = (unsigned int)((kl<<32)>>32);
    k ^= k2; k ^= k>>16; k ^= k>>8; k ^= k>>4; k ^= k>>2; k ^= k>>1;
    *(t+2*r+(int)(c/64)) |= ((uint64_t)(k&1)) << (c & 63);
  }
}
return 1;
}

int vvmult(uint64_t A[2], uint64_t B[2], uint64_t P[2], uint64_t *res)
{
// R = A * B in GF2 assuming field generator polymonial P.
int i, r127, bi;
uint64_t R[2];
const uint64_t one=0x0000000000000001;
R[0]=0; R[1]=0;
for (i=127;i>=0;i--) {
  r127 = (int)((R[1]>>63) & one);
  R[1] = (R[1]<<1) | ((R[0]>>63) & one); R[0] = R[0]<<1;
  bi = (int)((B[i>>6]>>(i%64)) & one);
  R[0] ^= ((r127==1)?P[0]:0) ^ ((bi==1)?A[0]:0);
  R[1] ^= ((r127==1)?P[1]:0) ^ ((bi==1)?A[1]:0);
}
*(res) = R[0]; *(res+1) = R[1];
}


int jumppoly(uint64_t p[2], __uint128_t JUMP, uint64_t *pj)
{
// Given charpoly p[2], and jump distance JUMP, calculate jump polynomial pj[2]
//
int i,j;
const uint64_t one=0x0000000000000001;
const __uint128_t one128=0x00000000000000000000000000000001;
uint64_t pp[2];
uint64_t v[2], vj[2], vjs[128][2];

//Characteristic polynomial 'p' can be a constant.
//static const uint64_t p[] = { 0x095b8f76579aa001, 0x0008828e513b43d5 }; // characteristic polynomial of xoroshiro-24-16-37
//static const uint64_t p[] = { 0x5fd66762f0e1c001, 0x00653ced7f29f88a }; // characteristic polynomial of xoroshiro-55-14-36

//form the jump vector vj
pp[0]=one << 1; pp[1]=0;
vj[0]=one; vj[1]=0;
for (i=0;i<128;i++) {
  // if JUMP bit i==1; multiply vector vj by p^(2^i);
  if (((JUMP>>i) & one128)==one128) {vvmult(vj, pp, p, &vj[0]);}
  // pp calculated on-the-fly here but could be precalculated and saved in a lookup table.
  // For jump distances of up to 2^30, an array of the first 30 constants are needed (for i=0 to 29)..
  vvmult(pp, pp, p, &pp[0]);
}

*(pj) =vj[0]; *(pj+1) =vj[1];

return 1;
}

int charpoly(uint64_t a[128][2], uint64_t *p)
{
// Using the Fedeev-LeVerrier algo, find charpoly. Requires 128-bit integer arithmetic.
int i,j,k,n;
__int128_t tr, temp;
__int128_t c[129];
uint64_t cbin[2];
const uint64_t one=0x0000000000000001;
const __uint128_t one128=0x00000000000000000000000000000001;
__int128_t m[128][128], am[128][128]; // working int matrices for finding charpoly only.

for (i=0;i<128;i++) {for (j=0;j<128;j++) {m[i][j]=0;}} // zero m
c[128] = one128;
for (k=0;k<=128;k++) {
  // am = a.m
  //printf("am = a.m, k=%d\n",k);
  for (i=0;i<128;i++) { //output (am) row
    for (j=0;j<128;j++) { //output (am) column
      am[i][j] = 0;
      for (n=0;n<128;n++) { //col a, row m
        if (((a[i][n>>6]>>(n%64))&one)==one) {am[i][j] += m[n][j];}
      }
    }
  }
  // c=(-1/k)*tr[am]
  tr=0; for (n=0;n<128;n++) {tr+=am[n][n];}
  c[128-k] = (k==0)?one128:-tr/((__int128_t)k);
  temp = (tr<0)?-tr:tr; i=0; while(temp!=0) {temp>>=1; i+=1;}
  // m = a.m + c.I
  for (i=0;i<128;i++) { //row
    for (j=0;j<128;j++) { //column
      m[i][j] = am[i][j];
      if (i==j) {m[i][j]+=c[128-k];}
    }
  }
}
cbin[0]=0; cbin[1]=0;
for (i=0;i<128;i++) {cbin[i>>6] |= ((c[i] & one) << (i%64));}
*(p) =cbin[0]; *(p+1) =cbin[1];

return 1;
}


int initim(uint64_t *t)
{
/* set to identity matrix */
int r,c;
for (r=0;r<64;r++) {
  *(t+2*r+0) = ((uint64_t)1)<<r;
  *(t+2*r+1) =  (uint64_t)0;
}
for (r=64;r<128;r++) {
  *(t+2*r+1) = ((uint64_t)1)<<(r-64);
  *(t+2*r+0) =  (uint64_t)0;
}

}

int initzm(uint64_t *t)
{
/* set to zero matrix */
int r,c;
for (r=0;r<128;r++) {
  *(t+2*r+0) = (uint64_t)0;
  *(t+2*r+1) = (uint64_t)0;
}

}

int printm(uint64_t m[128][2])
{
int r,c;
uint64_t bit;
printf("\n");
for (r=0;r<128;r++) {
  printf ("   ");
  for (c=0;c<128;c++) {
    bit = (m[r][c>>6]>>(c&63)) & ((uint64_t)1);
    printf("%s",(bit==((uint64_t)1))?"1 ":"0 ");
  }
  printf("\n");
}
return 1;
}

int summ(uint64_t m[128][2], uint64_t n[128][2], uint64_t *s)
{
int r;
for (r=0;r<128;r++) {
  *(s+2*r+0) = m[r][0] ^ n[r][0];
  *(s+2*r+1) = m[r][1] ^ n[r][1];
  }
return 1;
}

main()
{
int i, j, gen_type;
int x,y,z;
unsigned int shortjump;
uint64_t prngs[2];
uint64_t m[128][2];
const uint64_t one=0x0000000000000001;
const __uint128_t one128=0x00000000000000000000000000000001;
uint64_t cpoly1[2];
uint64_t cpolyj[2];

for (gen_type=0;gen_type<2;gen_type++) {

  if (gen_type==0) {x=24; y=16; z=37;
    printf("NB: NOT Graphcore's xoroxhiro LFSR. Shift distances %d,%d,%d. For comparison with code in http://prng.di.unimi.it/xoroshiro128plus.c\n",x,y,z);
    }  // used in http://prng.di.unimi.it/xoroshiro128plus.c
  else             {x=55; y=14; z=36;
    printf("Graphcore's xoroshiro LFSR. Shift distances %d,%d,%d\n",x,y,z);}  // used by Graphcore.

  // build a matrix for a single-step.
  for(i=0;i<128;i++){
    prngs[0]=(i>=64)?(uint64_t)0:one<<i; prngs[1]= (i<64)?(uint64_t)0:one<<(i-64);
    j = prngupdate_xoro(x,y,z,prngs, &prngs[0]);
    m[i][0]=prngs[0]; m[i][1]=prngs[1];
  }
  // calcuate characteristic polynomial
  j = charpoly(m, &cpoly1[0]);
  printf("characteristic polynomial of xoroshiro-%d-%d-%d = 0x",x,y,z);
  j=printvech(cpoly1);
  // calculate jump polynomial and constants for distances 2^64 and 2^96
  for(i=64;i<=96;i=i+32){
    j = jumppoly(cpoly1, one128<<i, &cpolyj[0]);
    printf("jump poly for xoroshiro-%d-%d-%d for dist 2^%3d = 0x",x,y,z,i);
    j = printvechc(cpolyj);
  }

  // an example short jump distance
  // edit for any other positive distance.
  // for negative distances, simply jump forwards by 2^128-1-distance.
  shortjump = 10000;
  j = jumppoly(cpoly1, (__uint128_t)shortjump, &cpolyj[0]);
  printf("jump poly for xoroshiro-%d-%d-%d for dist %d = 0x",x,y,z,shortjump);
  j = printvechc(cpolyj);

  //uint64_t shortjump1000 = (32ULL * 1024 * 1024 * 1024 * 1024) / (8 * 1000);
  //j = jumppoly(cpoly1, (__uint128_t)shortjump1000, &cpolyj[0]);
  //printf("jump poly for xoroshiro-%d-%d-%d for dist %lu = 0x",x,y,z,shortjump1000);
  //j = printvechc(cpolyj);
  //
  //uint64_t shortjump100 = (32ULL * 1024 * 1024 * 1024 * 1024) / (8 * 100);
  //j = jumppoly(cpoly1, (__uint128_t)shortjump100, &cpolyj[0]);
  //printf("jump poly for xoroshiro-%d-%d-%d for dist %lu = 0x",x,y,z,shortjump100);
  //j = printvechc(cpolyj);
  //
  //uint64_t shortjump10 = (32ULL * 1024 * 1024 * 1024 * 1024) / (8 * 10);
  //j = jumppoly(cpoly1, (__uint128_t)shortjump10, &cpolyj[0]);
  //printf("jump poly for xoroshiro-%d-%d-%d for dist %lu = 0x",x,y,z,shortjump10);
  //j = printvechc(cpolyj);
  //
  //uint64_t shortjump128mb = (128ULL * 1024 * 1024) / (8 * 2);
  //j = jumppoly(cpoly1, (__uint128_t)shortjump128mb, &cpolyj[0]);
  //printf("jump poly for xoroshiro-%d-%d-%d for dist %lu = 0x",x,y,z,shortjump128mb);
  //j = printvechc(cpolyj);
  //
  //uint64_t shortjump256mb = (256ULL * 1024 * 1024) / (8 * 2);
  //j = jumppoly(cpoly1, (__uint128_t)shortjump256mb, &cpolyj[0]);
  //printf("jump poly for xoroshiro-%d-%d-%d for dist %lu = 0x",x,y,z,shortjump256mb);
  //j = printvechc(cpolyj);
  //
  //uint64_t shortjump1024mb = (1024ULL * 1024 * 1024) / (8 * 2);
  //j = jumppoly(cpoly1, (__uint128_t)shortjump1024mb, &cpolyj[0]);
  //printf("jump poly for xoroshiro-%d-%d-%d for dist %lu = 0x",x,y,z,shortjump1024mb);
  //j = printvechc(cpolyj);

  //for(i=1;i<=16;i=i+1){
  //  j = jumppoly(cpoly1, one128<<i, &cpolyj[0]);
  //  printf("jump poly for xoroshiro-%d-%d-%d for dist 2^%3d = 0x",x,y,z,i);
  //  j = printvechc(cpolyj);
  //}

  // jump forwards 2^128-1. Should be equivalent to jumping forwards by 0
  j = jumppoly(cpoly1, (__uint128_t)(-one128), &cpolyj[0]);
  printf("jump poly for xoroshiro-%d-%d-%d for d=(2^128)-1 == zero (sanity check, jump polynomial should=1) 0x",x,y,z);
  j = printvech(cpolyj);
  printf("\n");

}
printf("End \n");
}
