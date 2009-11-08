export VERSION=$(shell git rev-parse HEAD | dd bs=8 count=1)
export CFLAGS=-Wall -g -I$(PWD)/include -DVERSION=\"$(VERSION)\"
export LDFLAGS=-L$(PWD)/cpu
export CC=gcc

export MAKE_INCDIR=$(PWD)/build
export BUGNES_ROOT=$(PWD)

# Order is important to get the dependencies right
SUBDIRS=cpu bin
SUBDIRS_TEST=test

all: subdirs subdirs-test cscope.out

cscope.out:
	cscope -Rb

clean: subdirs-clean
	$(RM) cscope.out

include $(MAKE_INCDIR)/tail.mk
