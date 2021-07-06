SOURCES = main.c arena.c assemble.c bank.c cell.c common.c process.c token.c vm.c x16.c

PROGRAM = COREWAR

CC65_TARGET = cx16

CC	= cl65 
CFLAGS 	= --cpu 65c02 -t $(CC65_TARGET) --create-dep $(<:.c=.d) -O -DX16
LDFLAGS	= -t $(CC65_TARGET) -m $(PROGRAM).map
OBJDIR  = .obj

#############################################
OBJECTS = $(SOURCES:%.c=$(OBJDIR)/%.o)

.SUFFIXES: 
all: clean $(PROGRAM)

ifneq (($MAKECMDGOALS),clean)
-include $(SOURCES:.c=.d)
endif

clean:
	$(RM) $(OBJECTS) $(SOURCES:.c=.d) $(PROGRAM) $(PROGRAM).map

.PHONY: all clean

$(PROGRAM): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJECTS): $(OBJDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<


