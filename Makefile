all := libnd-vanilla

LDLIBS-libnd-vanilla := -lxylem

# macOS ld rejects undefined symbols in shared libs, but WARN needs
# qsyslog: an engine-provided function pointer resolved at dlopen (Linux
# allows this by default). dynamic_lookup is the Darwin equivalent.
LDFLAGS-libnd-vanilla-Darwin += -undefined dynamic_lookup
FOLDER := nd
add-prefix-glob := ../nd-* ../axil-nd

-include ./../mk/include.mk

