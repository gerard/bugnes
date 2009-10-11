export CFLAGS=-Wall -g -I$(PWD)/include
export LDFLAGS=-L$(PWD)/cpu
export CC=gcc

export MAKE_INCDIR=$(PWD)/build
export BUGNES_ROOT=$(PWD)

# Order is important to get the dependencies right
SUBDIRS=cpu bin

all: subdirs cscope.out

cscope.out:
	cscope -Rb

clean: subdirs-clean
	$(RM) cscope.out

include $(MAKE_INCDIR)/tail.mk
