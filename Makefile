CC = gcc
CFLAGS  = -Wall -g
LFLAGS = `pkg-config --cflags glib-2.0`
LIBS = -lpaho-mqtt3as `pkg-config --libs glib-2.0` -lwiringPi -lwiringPiDev -lpthread -lm
INCLUDES = 
BUILD_DIR = build
SRCS_DIR = src
SRCS = $(wildcard $(SRCS_DIR)/*.c)
HEADERS_DIR = src
HEADERS = $(wildcard $(HEADERS_DIR)/*.h)
OBJS = $(SRCS:.c=.o)
MAIN_PROG = domo

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif


default: all

all: $(MAIN_PROG)
	@echo Doing $(MAIN_PROG)
	
$(MAIN_PROG): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/$(MAIN_PROG) $(OBJS) $(LFLAGS) $(LIBS)
	
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@ $(LFLAGS) $(LIBS)
clean: 
	rm -f $(BUILD_DIR)/* $(SRCS_DIR)/*.o $(SRCS_DIR)/*~
install: all	
	install -m 750 $(BUILD_DIR)/$(MAIN_PROG) $(DESTDIR)$(PREFIX)/sbin/$(MAIN_PROG)	
	install -m 755 examples/domo /etc/init.d/domo
	test -f /etc/domo.conf || install -m 421 examples/domo.conf /etc
	
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/sbin/$(MAIN_PROG)
depend:	$(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE make depend need it