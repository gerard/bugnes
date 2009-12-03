export VERSION := $(shell git rev-parse HEAD | dd bs=8 count=1 2> /dev/null)
export BUGNES_ROOT = $(PWD)

include build/head.mk

cscope.out:
	@echo CSCOPE
	@cscope -Rb

SUBDIRS = cpu/6502 bin/simplenes bin/simple6502 test
TARGETS = cscope.out
EXTRA = cscope.out
EXTRA_CLEAN = cscope.out

include build/tail.mk
