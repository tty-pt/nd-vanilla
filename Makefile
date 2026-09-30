all := libnd-vanilla

LDLIBS-libnd-vanilla := -lxylem

CFLAGS += -I$(shell cd .. && pwd)/axil-nd/include

-include ./../mk/include.mk
