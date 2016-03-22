IDIR =.
SRC_DIR=.
ODIR=.

CC=gcc
CFLAGS=-I$(IDIR) -g -Wall


DEPS = $($(SRC_DIR)/%,$(IDIR)/%,$(_DEPS))

OBJ =	$(ODIR)/http_client.o \
		$(ODIR)/http_client_test.o


SRC =   $(SRC_DIR)/http_client.c \
        $(SRC_DIR)/http_client_test.c


INCLUDES = \
        -I$(IDIR) \
        -I$(SRCDIR)

$(ODIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


httpclienttest: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
