#
# Makefile for libwifitest
#

# definitions
# ------------------------------------------------------------------------------

# wifitest tool info
PROGRAM				 = wifitest

LDFLAGS := -lutil -lm
# compiling and linking
# ------------------------------------------------------------------------------
all: $(PROGRAM)
clean:
	-rm -f $(PROGRAM)
$(PROGRAM): main.c lib.c libwifitest.c 
	$(CC) ${CFLAGS} -o $@ $< lib.c libwifitest.c $(LDFLAGS)
