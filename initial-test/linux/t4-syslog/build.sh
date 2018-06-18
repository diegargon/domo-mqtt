#!/bin/sh
rm -f syslog syslog.o
clear
gcc -o syslog syslog.c -Wall -g