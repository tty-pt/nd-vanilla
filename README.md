# axil-nd-vanilla

`nd-vanilla` for [axil-nd](../axil-nd), ported from SIC to libxylem.

Places a newly connected player in the world. This is mostly a **dead handler
story**: only one of the three handlers this module is named for actually fires.

## Install

```sh
make install
```

Installs one file:

```
lib/libnd-vanilla.so
```

There is deliberately no `lib/nd-vanilla.so` symlink. `xy_load()` appends `.so`
itself and does not retry with a `lib` prefix, so the engine's `mods.load` names
this module `libnd-vanilla` and `dlopen`s `libnd-vanilla.so`. A soname symlink
would also have been silently dropped from the OpenBSD package: `tty-pt/ci`
builds the packing list from `find usr -type f`, which never lists a symlink, so
the package would have shipped the library under one name and asked the loader
for another.

Also packaged for deb, apk, rpm, brew and openbsd from a `v*` tag.

It installs no header because it exports no API — `on_new_player` is an event
hook the engine declares itself in `nd/hooks.h`, not something another module
calls.

## Build from source

```sh
make
```

Needs [libxylem](https://github.com/tty-pt/libxylem) (headers already in
`$(PREFIX)/include`) and the engine's game API, `<nd/xy.h>`, from either an
`axil-nd` checkout beside this repo or an installed `axil-nd`:

```sh
git clone https://github.com/tty-pt/nd-vanilla && cd nd-vanilla
git clone https://github.com/tty-pt/axil-nd ../axil-nd
make
```

`<nd/xy.h>` installs to `$(PREFIX)/include/nd/`, the same directory that already
carries `<ttypt/xy.h>`, so an **installed** engine needs no `-I` of its own here.
Against a checkout beside this repo it is `-I../axil-nd/include`; both paths are
on `CFLAGS` at once and a missing `-I` is ignored, so the same command works
either way.

## What it does

* `on_new_player(player_ref)` calls `st_teleport(player_ref, 0)`, putting a newly
  connected player into the world, and logs the ref it was given.
* Two further handlers are **kept unported**, and that is the interesting part:

  | Handler | State |
  |---|---|
  | `on_new_player` | LIVE — the engine fires it from `src/nd_events.c` |
  | `on_birth` | DEAD — not an event at all |
  | `on_death` | DEAD — likewise |

  "Not an event at all" is the sharper form and it matters for how you revive
  one. These are not hooks that exist with nobody calling them (compare
  `on_status`, which *is* a real event, fired by the `status` command in
  `src/entity.c`): the engine has no `on_birth`/`on_death` `XY_DEF` and no
  `nd_evt_birth`/`nd_evt_death` wrapper to call. So there is no firing site to
  find and re-wire — the event has to be created.

## In-Game Commands (`man-src/`)

None directly. Governs player genesis and spawn placement.

## Packaged Assets (`art/`)

Baseline entity and item skeletons.

## Wire Protocol & BCP Integration

Teleports newly authenticated players to room reference 0, initiating the first `BCP_ROOM` and `BCP_VIEW` frames sent to the client.

## Modder Extension Guide

To alter where new players begin their journey or provide starter items:
1. Adjust `on_new_player()` to teleport players to a custom starting sanctuary room.
2. Grant starter equipment or gold through the `HD_OBJ` API before dispatching.

## Testing

There is no `test.sh` here. Behaviour is asserted by the engine's own suite,
which builds every module in its `mods.load`, boots, and greps stderr:

```sh
cd ../axil-nd
make && ./test.sh
```

The suite asserts `nd-vanilla: on_new_player teleported`.

## Notes from the port

* **The include changed spelling.** It was `"papi/nd-xy.h"`, a file that no
  longer exists in any checkout: the engine moved its module-facing tree from
  `papi/` to `nd/`, and `papi/` now holds only `nd.h`. The module could not
  compile until this was fixed.
* **This module is not what the plan said it was.** MODS.md §6 claimed vanilla
  "proves on_leave wiring"; `on_leave` lives in `nd-equip`, not here, and is
  Phase 2. The source is the authority.
* **The two dead handlers ship commented out rather than ported.** Porting them
  would compile, but a handler nothing ever calls is code that looks alive and
  never runs, and it would drag `object_move()`/`room_clean()` into this
  module's surface for no reason. They are preserved verbatim in comments so the
  decision stays reviewable and the port remains a faithful transcription. Note
  that `on_death` was *already wrong* for its return type — it returns
  `NOTHING`, i.e. `(unsigned) -1`, out of an `int` hook — so it must not be
  ported as-is.
* They were already unreachable in the old engine (`git grep` at 1985533), so no
  behaviour is being preserved either way. Inventing the events would be a new
  engine feature, not a port. To revive one: add its `XY_DEF` in
  `src/nd_events.c`, wire a firing site, and port it.

## License

BSD 2-Clause, carried over from `tty-pt/nd-vanilla`. See `LICENSE`.
