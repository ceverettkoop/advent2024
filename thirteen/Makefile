CC=gcc
IFLAGS = -I../include/
CFLAGS= $(IFLAGS) 
DEBUGFLAGS= -ggdb
LDFLAGS=
SANFLAGS= -fsanitize=undefined -fsanitize=address
CHECKFLAGS=-Wall -Wformat -Wformat=2 -Wimplicit-fallthrough \
-D_GLIBCXX_ASSERTIONS \
-fstack-clash-protection -fstack-protector-strong \
-Wl,-z,nodlopen -Wl,-z,noexecstack \
-Wl,-z,relro -Wl,-z,now \
-Werror=implicit -Werror=incompatible-pointer-types -Werror=int-conversion \
-Wtrampolines

ODIR=obj
SRCDIR=src
BINNAME=advent

SRCS := $(wildcard $(SRCDIR)/*.c)
_OBJ := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)%.o,$(SRCS))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

ifeq ($(NOWARN),TRUE)
	CHECKFLAGS = 
endif

ifeq ($(BUILD),RELEASE)
	CFLAGS += -O3 -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 -Wno-unused-result
else
	CFLAGS += $(DEBUGFLAGS)
	CFLAGS += $(CHECKFLAGS)
endif

ifeq ($(SANITIZE), STATIC)
	CFLAGS += $(SANFLAGS)
	LDFLAGS += $(SANFLAGS) -static-libasan
endif

ifeq ($(SANITIZE), TRUE)
	CFLAGS += $(SANFLAGS)
	LDFLAGS += $(SANFLAGS)
endif

$(ODIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS) $(PLATFLAGS)

simplereport:$(OBJ)
	$(CC) -o $(BINNAME) $(OBJ) $(LDFLAGS) $(LDLIBS) $(PLATFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o