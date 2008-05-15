
ifdef DISABLE_DEBUG_PRINT
    DDPFLAGS=-DDISABLE_DEBUG_PRINT=1
else
    DDPFLAGS=
endif

ifdef DEBUG
    DEBUGFLAGS=-g -ggdb -dH -D_DEBUG_=1 -DDEBUG=1
    STRIP=@echo
else
    DEBUGFLAGS=
    STRIP=strip
endif

ifdef NOASSERT
    ASSERTFLAGS=-DDISABLE_ASSERT=1
else
    ASSERTFLAGS=
endif

CC=gcc
CFLAGS=-W -Wall -Wno-unused -O3 $(DDPFLAGS) $(DEBUGFLAGS) $(ASSERTFLAGS)
LDFLAGS=

SRC=protocol.c common.c vector.c md5.c misc.c

SRC1=eftpd.c server.c server_act.c user.c $(SRC)
OBJS1=$(SRC1:.c=.o)
EXE1=eftpd

SRC2= $(SRC)
OBJS2=$(SRC2:.c=.o)
EXE2=eftp


all: $(EXE1) $(EXE2)

common.o: protocol.h md5.h common.h bool.h assert.h
eftpd.o: server.h user.h protocol.h misc.h bool.h assert.h
md5.o: md5.h assert.h
misc.o: bool.h misc.h
protocol.o: assert.h protocol.h
server.o: user.h common.h server.h assert.h vector.h
server_act.o: protocol.h server_act.h assert.h vector.h
user.o: vector.h bool.h protocol.h user.h assert.h
vector.o: vector.h assert.h

$(EXE1): $(OBJS1)
	$(CC) $(OBJS1) -o $@ $(LDFLAGS)
	$(STRIP) $@ > /dev/null

$(EXE2): $(OBJS2)
	$(CC) $(OBJS2) -o $@ $(LDFLAGS)
	$(STRIP) $@ > /dev/null

.PHONY: clean mrproper

mrproper: clean
	@rm -f $(EXE1) $(EXE2)
clean:
	@rm -f *.o */*.o *~ core *.core core.* *.tmp

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)


