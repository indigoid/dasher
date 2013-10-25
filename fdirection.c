#include <math.h>
#include <stdio.h>

#include "fdirection.h"

/* given reference point r and position p, return
 *   +1 if p < r
 *   -1 if p > r
 *    0 if abs(p-r) < NEARBY
 */
int fdirection(float r, float p) {
	if (fabs(r-p) < NEARBY)
		return 0;
	if (p < r)
		return 1;
	if (p > r)
		return -1;
	return 0;
}

#ifdef BUILDTEST

const char* directions[] = {
	"GO WEST",
	"ARRIVED",
	"GO EAST"
};

int main(int argc, char** argv) {
	float pos = -10.0;
	float ref = 3.141596;
	int dir;
	while ((dir = fdirection(ref,pos)) != 0) {
		pos += 0.0005;
		printf("ref=%.6f; pos=%.6f; dir=%s\n",
			ref, pos, directions[dir+1]);
	}
	printf("ref=%.6f; pos=%.6f; dir=%s\n",
		ref, pos, directions[dir+1]);
	return 0;
}

#endif
