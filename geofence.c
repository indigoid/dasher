#include <stddef.h>
#include <assert.h>
#include <math.h>

#include "geofence.h"

/*
 * see: http://en.wikipedia.org/wiki/law_of_haversines
 * originally C++ code from Julien Cayzac's blog, link above.
 * converted to plain old C, typos fixed, test case added
 */

/* the usual pi/180 constant */
#define DEG_TO_RAD 0.017453292519943295769236907684886
/* earth's quadtratic mean radius for wgs-84 */
#define EARTH_RADIUS_IN_METRES 6372797.560856

static double arc_in_radians(const struct geo* from, const struct geo* to) {
	assert(from != NULL);
	assert(to != NULL);
	double latitudearc  = (from->latitude - to->latitude) * DEG_TO_RAD;
	double longitudearc = (from->longitude - to->longitude) * DEG_TO_RAD;
	double latitudeh = sin(latitudearc * 0.5);
	latitudeh *= latitudeh;
	double longtitudeh = sin(longitudearc * 0.5);
	longtitudeh *= longtitudeh;
	double tmp = cos(from->latitude*DEG_TO_RAD) * cos(to->latitude*DEG_TO_RAD);
	return 2.0 * asin(sqrt(latitudeh + tmp*longtitudeh));
}

/* computes the distance, in meters, between two wgs-84 positions. */
double distance_in_metres(const struct geo* from, const struct geo* to) {
    return EARTH_RADIUS_IN_METRES*arc_in_radians(from, to);
}

#ifdef BUILDTEST

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define INCR 0.0005

int main(int argc, char** argv) {
	struct geo p = { .latitude = 0.0, .longitude = 0.0 };
	struct geo q = p;
	double dist;
	srand(time(NULL));
	while ((dist = distance_in_metres(&p, &q)) < 1000.0) {
		if (rand() % 10 > 5)
			q.latitude  += INCR;
		else
			q.longitude += INCR;
		printf("moved to %.4f/%.4f; distance from p is now %.1f metres\n",
			q.latitude, q.longitude, dist);
	}
	printf("final coordinates: %.4f/%.4f\n",
		q.latitude, q.longitude);
	return 0;
}

#endif
