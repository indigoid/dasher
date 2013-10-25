#ifndef IG_GEOFENCE_H__
#define IG_GEOFENCE_H__

struct geo {
	float			latitude;
	float			longitude;
	
};

float distance_in_metres(const struct geo* from, const struct geo* to);

#endif /* IG_GEOFENCE_H__ */
