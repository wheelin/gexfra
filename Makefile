CC=gcc
CFLAGS=-Wall -Wextra  -g -O2 -std=c11
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=test

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
	
	
clean:
	@rm -f $(OBJECTS)
	@rm -f *.d *~
	
clean_all: clean
	@rm -f $(EXECUTABLE)	

-include *.d
