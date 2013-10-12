#ifndef IG_GEOFENCE_H__
#define IG_GEOFENCE_H__

struct geo {
	double			latitude;
	double			longitude;
	
};

double distance_in_metres(const struct geo* from, const struct geo* to);

#endif /* IG_GEOFENCE_H__ */
