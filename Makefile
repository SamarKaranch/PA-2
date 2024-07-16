# Enable compiler warnings by default
CFLAGS ?= -Wall -Wextra

# Corresponding object file for each .c file
OBJECTS = chash.o command_parser.o thread_launcher.o concurrent_list.o rwlock.o

# Default target
.PHONY: all
all: chash

# Link all objects into executable
chash: $(OBJECTS)

.PHONY: clean
clean:
	-rm -f chash $(OBJECTS)
