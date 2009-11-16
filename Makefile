export VERSION:=$(shell git rev-parse HEAD | dd bs=8 count=1 2> /dev/null)
export CFLAGS=-Wall -O0 -g -I$(PWD)/include -DVERSION=\"$(VERSION)\"
export LDFLAGS=-L$(PWD)/cpu
export CC=gcc

export MAKE_INCDIR=$(PWD)/build
export BUGNES_ROOT=$(PWD)

# Order is important to get the dependencies right
SUBDIRS=cpu bin test

all: subdirs cscope.out

cscope.out:
	@echo CSCOPE
	@cscope -Rb

clean: subdirs-clean
	@echo CLEAN
	@$(RM) cscope.out

include $(MAKE_INCDIR)/tail.mk
