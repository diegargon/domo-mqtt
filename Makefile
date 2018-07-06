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
MAIN = domo


default: all

all: $(MAIN)
	@echo Doing $(MAIN)
	
$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/$(MAIN) $(OBJS) $(LFLAGS) $(LIBS)
	
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@ $(LFLAGS) $(LIBS)
clean: 
	rm -f $(BUILD_DIR)/* $(SRCS_DIR)/*.o $(SRCS_DIR)/*~
	
depend:	($SRCS)
	makedpend $(INCLUDES) $^

# DO NOT DELETE THIS LINE make depend need it