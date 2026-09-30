/* main.c — nd-vanilla, ported to libxylem.
 *
 * Places a newly connected player in the world.
 *
 * This module is mostly a DEAD HANDLER STORY, and the original source is not
 * the plan's summary of it (MODS.md §6 said vanilla "proves on_leave
 * wiring"; on_leave lives in nd-equip, not here, and is Phase 2). What vanilla
 * actually carries is:
 *
 *   on_new_player  LIVE   -- the engine fires it (src/nd_events.c, nd_evt_new_player).
 *   on_birth       DEAD   -- not an event at all: no XY_DEF, no nd_evt_* wrapper.
 *   on_death       DEAD   -- likewise.
 *
 * "Not an event at all" is the sharper form and it matters for how you revive
 * one. These are not hooks that exist with nobody calling them (compare
 * on_status, which IS a real event, fired by the `status` command in
 * src/entity.c:236): the engine has no on_birth/on_death XY_DEF and no
 * nd_evt_birth/nd_evt_death wrapper to call. So there is no firing site to
 * find and re-wire -- the event has to be created.
 *
 * They were already unreachable in the old engine (git grep at 1985533), so
 * there is no behaviour being preserved either way, and inventing the events
 * would be a new engine feature rather than a port.
 *
 * Per MODS.md §6's rule (a dead handler ships with a "no firing site"
 * comment), the two dead handlers are kept verbatim and commented rather than
 * ported: porting them would compile, but a handler nothing ever calls is code
 * that looks alive and never runs, and it would drag object_move()/room_clean()
 * into the module's surface for no reason. They are preserved here so the
 * decision is reviewable and the port stays a faithful transcription.
 *
 * To revive one, add its XY_DEF in src/nd_events.c, wire a firing site, and
 * port it.
 *
 * This TU XY_IMPLs on_new_player and so must NOT include nd/hooks.h.
 */

#include <ttypt/xy-mod.h>

#include <nd/xy.h>

XY_MODULE_API void
xy_install(void)
{
	WARN("nd-vanilla: xy_install, on_new_player registered\n");
}

XY_IMPL(int, on_new_player, unsigned, player_ref)
{
	st_teleport(player_ref, 0);
	WARN("nd-vanilla: on_new_player teleported ref %u\n", player_ref);
	return 0;
}

/* --- dead since the old nd: no firing site in the engine --------------- */

/* No firing site: dead since the old nd. Kept unported, see above.
 *
 * int on_birth(unsigned ref, uint64_t v) {
 * 	OBJ obj;
 * 	nd_get(HD_OBJ, &obj, &ref);
 * 	if (obj.flags & OF_PLAYER)
 * 		st_teleport(ref, 0);
 * 	return 0;
 * }
 */

/* No firing site: dead since the old nd. Kept unported, see above. Note this
 * one is a `return NOTHING` (`(unsigned) -1`) out of an int hook, i.e. it was
 * already wrong for its return type; do not port it as-is.
 *
 * int on_death(unsigned ref) {
 * 	OBJ obj, loc;
 *
 * 	nd_get(HD_OBJ, &obj, &ref);
 * 	nd_get(HD_OBJ, &loc, &obj.location);
 *
 * 	if ((((ROO *) loc.data)->flags & RF_TEMP) && !(obj.flags & OF_PLAYER)) {
 * 		object_move(ref, NOTHING);
 * 		room_clean(obj.location);
 * 		return NOTHING;
 * 	}
 *
 * 	st_teleport(ref, 0);
 * 	return 0;
 * }
 */
