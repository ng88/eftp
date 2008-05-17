
ifdef DISABLE_DEBUG_PRINT
    DDPFLAGS=-DDISABLE_DEBUG_PRINT=1
else
    DDPFLAGS=
endif

ifdef ENABLE_RELIABILITY
    ER=-DENABLE_RELIABILITY=1
else
    ER=
endif

ifdef ENABLE_UDP_ERRORS
    EU=-DENABLE_UDP_ERRORS=1
else
    EU=
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
CFLAGS=-W -Wall -Wno-unused -O3 $(EU) $(ER) $(DDPFLAGS) $(DEBUGFLAGS) $(ASSERTFLAGS)
LDFLAGS=

SRC=protocol.c common.c vector.c misc.c

SRC1=eftpd.c server.c server_act.c user.c $(SRC)
OBJS1=$(SRC1:.c=.o)
EXE1=eftpd

SRC2=eftp.c client.c $(SRC)
OBJS2=$(SRC2:.c=.o)
EXE2=eftp


all: $(EXE1) $(EXE2)

common.o: protocol.h common.h bool.h assert.h
eftpd.o: server.h user.h protocol.h misc.h bool.h assert.h
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


