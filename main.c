#include <nd/nd.h>

int on_birth(unsigned ref, uint64_t v __attribute__((unused))) {
	OBJ obj;
	nd_get(HD_OBJ, &obj, &ref);
	if (obj.flags & OF_PLAYER)
		st_teleport(ref, 0);
	return 0;
}

int on_death(unsigned ref) {
	OBJ obj, loc;

	nd_get(HD_OBJ, &obj, &ref);
	nd_get(HD_OBJ, &loc, &obj.location);

	if ((((ROO *) loc.data)->flags & RF_TEMP) && !(obj.flags & OF_PLAYER)) {
		object_move(ref, NOTHING);
		room_clean(obj.location);
		return NOTHING;
	}

	st_teleport(ref, 0);
	return 0;
}

int on_leave(unsigned ref, unsigned loc_ref) {
	OBJ obj;
	nd_get(HD_OBJ, &obj, &ref);
	if (!(obj.flags & OF_PLAYER))
		return 1;
	room_clean(loc_ref);
	return 0;
}

int on_new_player(unsigned player_ref) {
	st_teleport(player_ref, 0);
	return 0;
}
