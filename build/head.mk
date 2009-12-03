# Some standard tools
CC = gcc
LD = gcc
AR = ar cq
RM = rm -rf
MKDIR = mkdir -p
RMDIR = rmdir 2> /dev/null
MAKE = make -I$(BUGNES_ROOT)

# If V is defined, we want verbose output, but shutting it up is a good thing
# as a default
ifndef V
.SILENT:
endif

# Some standard flags
CFLAGS = -Wall -O0 -g -DVERSION=\"$(VERSION)\"
LDFLAGS =

PROJECT_PATH = $(subst $(BUGNES_ROOT)/,,$(shell pwd))
TESTDIR_TO_IMP = $(BUGNES_ROOT)/$(subst $(BUGNES_ROOT)/test,,$(shell pwd))


# Default target: do the subdirectories, and then recurse to itself (this is
# needed so variables are reevaluated
all: subdirs print-recurse
	@$(MAKE) self

# Print a nice blue directory when recursing
ifeq ($(PROJECT_PATH), $(BUGNES_ROOT))
PROJECT_PATH=
print-recurse:
	@printf "==> \033[1;34m.\033[0m\n"
else
print-recurse:
	@printf "==> \033[1;34m$(PROJECT_PATH)\033[0m\n"
endif

.PHONY: all print-recurse
