#!/bin/sh
rm -f ads1115-i2c ads1115a *.o
#clear
gcc -o ads1115-i2c ads1115-i2c.c -Wall -g -lwiringPi -lwiringPiDev
#gcc -o ads1115a ads1115a.c -Wall -g -lwiringPi -lwiringPiDev
