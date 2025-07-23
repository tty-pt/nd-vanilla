#include <nd/nd.h>

int on_birth(unsigned ref, uint64_t v) {
	ENT ent = ent_get(ref);
	if (ent.flags & EF_PLAYER)
		st_teleport(ref, 0);
	return 0;
}

int on_death(unsigned ref) {
	OBJ obj, loc;

	nd_get(HD_OBJ, &obj, &ref);
	nd_get(HD_OBJ, &loc, &obj.location);

	if ((((ROO *) loc.data)->flags & RF_TEMP) && !(ent_get(ref).flags & EF_PLAYER)) {
		object_move(ref, NOTHING);
		room_clean(obj.location);
		return NOTHING;
	}

	st_teleport(ref, 0);
	return 0;
}

int on_new_player(unsigned player_ref) {
	st_teleport(player_ref, 0);
	return 0;
}
