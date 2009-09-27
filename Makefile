export CFLAGS=-Wall -g -I$(PWD)/include
export LDFLAGS=-L$(PWD)/cpu
export CC=gcc

export MAKE_INCDIR=$(PWD)/build
export BUGNES_ROOT=$(PWD)

# Order is important to get the dependencies right
SUBDIRS=cpu bin

all: subdirs
clean: subdirs-clean

include $(MAKE_INCDIR)/tail.mk
