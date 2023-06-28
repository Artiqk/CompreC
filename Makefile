CC = gcc
CFLAGS  = -Iinclude -Wall
LIBS    = -lzip
SRCDIR  = src
OBJDIR  = obj
BINDIR  = bin
INCLUDE = include

_DEPS = zip_tools.h
DEPS = $(patsubst %,$(INCLUDE)/%,$(_DEPS))

_OBJ = main.o zip_tools.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	mkdir -p $(OBJDIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BINDIR)/main: $(OBJ)
	mkdir -p $(BINDIR)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLUDE)/*~ $(BINDIR)/main
