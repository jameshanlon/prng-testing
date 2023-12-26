/*
 * Copyright (c) Stephen Felix 2022.
 */

#include "float.h"
#include "math.h"
#include "stdio.h"
#define __STDC_FORMAT_MACROS
#include "inttypes.h"
#include "jump.h"

int prngupdate_xoro(int x, int y, int z, uint64_t t[2], uint64_t *tp) {
  // Variant used by GC : x,y,z = 55,14,36
  // Alternative variant: x,y,z = 24,16,37
  uint64_t s0, s1;
  s0 = t[0];
  s1 = t[1];
  s1 ^= s0;
  s0 = ((s0 << x) | (s0 >> (64 - x))) ^ s1 ^ (s1 << y);
  s1 = ((s1 << z) | (s1 >> (64 - z)));
  *(tp + 0) = s0;
  *(tp + 1) = s1;
  return 1;
}

int printvec(uint64_t t[2]) {
  int c, b, i;
  i = 0;
  for (c = 127; c >= 0; c--) {
    b = (t[(int)(c / 64)] >> (c & 63)) & 1;
    printf("%d", b);
    i += b;
  }
  printf("   %d ones\n", i);
  return 1;
}

int printvech(uint64_t t[2]) {
  int c, b;
  for (c = 31; c >= 0; c--) {
    b = (t[(int)(c / 16)] >> ((c * 4) % 64)) & 15;
    printf("%1x", b);
  }
  printf("\n");
  return 1;
}

int printvechc(uint64_t t[2]) {
  int c, b;
  for (c = 31; c >= 0; c--) {
    b = (t[(int)(c / 16)] >> ((c * 4) % 64)) & 15;
    printf("%1x", b);
  }
  printf("   // static const uint64_t JUMP[] = { 0x");
  for (c = 15; c >= 0; c--) {
    b = (t[(int)(c / 16)] >> ((c * 4) % 64)) & 15;
    printf("%1x", b);
  }
  printf(", 0x");
  for (c = 31; c >= 16; c--) {
    b = (t[(int)(c / 16)] >> ((c * 4) % 64)) & 15;
    printf("%1x", b);
  }
  printf(" };\n");
  return 1;
}

int transpose(uint64_t m[128][2], uint64_t *t) {
  int r, c;

  for (r = 0; r < 128; r++) {
    *(t + 2 * r) = 0;
    *(t + 2 * r + 1) = 0;
    for (c = 0; c < 128; c++) {
      *(t + 2 * r + (int)(c / 64)) |= ((m[c][(int)(r / 64)] >> (r & 63)) & 1)
                                      << (c & 63);
    }
  }
  return 1;
}

int mmult(uint64_t m[128][2], uint64_t n[128][2], uint64_t *t) {
  int r, c, k, k2;
  uint64_t bit[2];
  uint64_t nt[128][2];
  uint64_t kl;

  r = transpose(n, &nt[0][0]);

  for (r = 0; r < 128; r++) {
    *(t + 2 * r) = 0;
    *(t + 2 * r + 1) = 0;
    for (c = 0; c < 128; c++) {
      bit[0] = m[r][0] & nt[c][0];
      bit[1] = m[r][1] & nt[c][1];
      kl = bit[0] ^ bit[1];
      k = (unsigned int)(kl >> 32);
      k2 = (unsigned int)((kl << 32) >> 32);
      k ^= k2;
      k ^= k >> 16;
      k ^= k >> 8;
      k ^= k >> 4;
      k ^= k >> 2;
      k ^= k >> 1;
      *(t + 2 * r + (int)(c / 64)) |= ((uint64_t)(k & 1)) << (c & 63);
    }
  }
  return 1;
}

void vvmult(uint64_t A[2], uint64_t B[2], uint64_t P[2], uint64_t *res) {
  // R = A * B in GF2 assuming field generator polymonial P.
  int i, r127, bi;
  uint64_t R[2];
  const uint64_t one = 0x0000000000000001;
  R[0] = 0;
  R[1] = 0;
  for (i = 127; i >= 0; i--) {
    r127 = (int)((R[1] >> 63) & one);
    R[1] = (R[1] << 1) | ((R[0] >> 63) & one);
    R[0] = R[0] << 1;
    bi = (int)((B[i >> 6] >> (i % 64)) & one);
    R[0] ^= ((r127 == 1) ? P[0] : 0) ^ ((bi == 1) ? A[0] : 0);
    R[1] ^= ((r127 == 1) ? P[1] : 0) ^ ((bi == 1) ? A[1] : 0);
  }
  *(res) = R[0];
  *(res + 1) = R[1];
}

int jumppoly(uint64_t p[2], __uint128_t JUMP, uint64_t *pj) {
  // Given charpoly p[2], and jump distance JUMP, calculate jump polynomial
  // pj[2]
  //
  int i;
  const uint64_t one = 0x0000000000000001;
  const __uint128_t one128 = 0x00000000000000000000000000000001;
  uint64_t pp[2];
  uint64_t vj[2];

  // Characteristic polynomial 'p' can be a constant.
  // static const uint64_t p[] = { 0x095b8f76579aa001, 0x0008828e513b43d5 }; //
  // characteristic polynomial of xoroshiro-24-16-37 static const uint64_t p[] =
  // { 0x5fd66762f0e1c001, 0x00653ced7f29f88a }; // characteristic polynomial of
  // xoroshiro-55-14-36

  // form the jump vector vj
  pp[0] = one << 1;
  pp[1] = 0;
  vj[0] = one;
  vj[1] = 0;
  for (i = 0; i < 128; i++) {
    // if JUMP bit i==1; multiply vector vj by p^(2^i);
    if (((JUMP >> i) & one128) == one128) {
      vvmult(vj, pp, p, &vj[0]);
    }
    // pp calculated on-the-fly here but could be precalculated and saved in a
    // lookup table. For jump distances of up to 2^30, an array of the first 30
    // constants are needed (for i=0 to 29)..
    vvmult(pp, pp, p, &pp[0]);
  }

  *(pj) = vj[0];
  *(pj + 1) = vj[1];

  return 1;
}

int charpoly(uint64_t a[128][2], uint64_t *p) {
  // Using the Fedeev-LeVerrier algo, find charpoly. Requires 128-bit integer
  // arithmetic.
  int i, j, k, n;
  __int128_t tr, temp;
  __int128_t c[129];
  uint64_t cbin[2];
  const uint64_t one = 0x0000000000000001;
  const __uint128_t one128 = 0x00000000000000000000000000000001;
  __int128_t m[128][128],
      am[128][128]; // working int matrices for finding charpoly only.

  for (i = 0; i < 128; i++) {
    for (j = 0; j < 128; j++) {
      m[i][j] = 0;
    }
  } // zero m
  c[128] = one128;
  for (k = 0; k <= 128; k++) {
    // am = a.m
    // printf("am = a.m, k=%d\n",k);
    for (i = 0; i < 128; i++) {   // output (am) row
      for (j = 0; j < 128; j++) { // output (am) column
        am[i][j] = 0;
        for (n = 0; n < 128; n++) { // col a, row m
          if (((a[i][n >> 6] >> (n % 64)) & one) == one) {
            am[i][j] += m[n][j];
          }
        }
      }
    }
    // c=(-1/k)*tr[am]
    tr = 0;
    for (n = 0; n < 128; n++) {
      tr += am[n][n];
    }
    c[128 - k] = (k == 0) ? one128 : -tr / ((__int128_t)k);
    temp = (tr < 0) ? -tr : tr;
    i = 0;
    while (temp != 0) {
      temp >>= 1;
      i += 1;
    }
    // m = a.m + c.I
    for (i = 0; i < 128; i++) {   // row
      for (j = 0; j < 128; j++) { // column
        m[i][j] = am[i][j];
        if (i == j) {
          m[i][j] += c[128 - k];
        }
      }
    }
  }
  cbin[0] = 0;
  cbin[1] = 0;
  for (i = 0; i < 128; i++) {
    cbin[i >> 6] |= ((c[i] & one) << (i % 64));
  }
  *(p) = cbin[0];
  *(p + 1) = cbin[1];

  return 1;
}

void initim(uint64_t *t) {
  /* set to identity matrix */
  for (int r = 0; r < 64; r++) {
    *(t + 2 * r + 0) = ((uint64_t)1) << r;
    *(t + 2 * r + 1) = (uint64_t)0;
  }
  for (int r = 64; r < 128; r++) {
    *(t + 2 * r + 1) = ((uint64_t)1) << (r - 64);
    *(t + 2 * r + 0) = (uint64_t)0;
  }
}

void initzm(uint64_t *t) {
  /* set to zero matrix */
  for (int r = 0; r < 128; r++) {
    *(t + 2 * r + 0) = (uint64_t)0;
    *(t + 2 * r + 1) = (uint64_t)0;
  }
}

int printm(uint64_t m[128][2]) {
  int r, c;
  uint64_t bit;
  printf("\n");
  for (r = 0; r < 128; r++) {
    printf("   ");
    for (c = 0; c < 128; c++) {
      bit = (m[r][c >> 6] >> (c & 63)) & ((uint64_t)1);
      printf("%s", (bit == ((uint64_t)1)) ? "1 " : "0 ");
    }
    printf("\n");
  }
  return 1;
}

int summ(uint64_t m[128][2], uint64_t n[128][2], uint64_t *s) {
  int r;
  for (r = 0; r < 128; r++) {
    *(s + 2 * r + 0) = m[r][0] ^ n[r][0];
    *(s + 2 * r + 1) = m[r][1] ^ n[r][1];
  }
  return 1;
}
