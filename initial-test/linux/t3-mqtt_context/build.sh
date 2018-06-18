#!/bin/sh

rm -f *.o mqtt_context
clear
gcc -o mqtt_context mqtt_context.c -O0 -g -Wall -fbuiltin -lpaho-mqtt3as `pkg-config --cflags --libs glib-2.0`
