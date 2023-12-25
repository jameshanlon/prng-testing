#include "stdint.h"

int printvech(uint64_t t[2]);
int prngupdate_xoro(int x, int y, int z, uint64_t t[2], uint64_t *tp);
int jumppoly(uint64_t p[2], __uint128_t JUMP, uint64_t *pj);
int charpoly(uint64_t a[128][2], uint64_t *p);
