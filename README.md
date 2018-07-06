# domo-mqtt
Domotica (Home automation)


I begin testing mqtt for home automation, here you can find some initial simple test files
i got used to linux/c again and a main application


Will use:

mosquitto (MQTT broker/server running in a linux server)

paho.mqtt.c (Async c libray)

OrangePI (Raspberry PI clone, programming in C)

ESP8266 (probably pragrammed with arduino)


# requeriments

WiringPI, i have a orange pi H3 that work
git clone https://github.com/zhaolei/WiringOP.git -b h3 
glib-2.0


# Use and compile

Compile:
$ make
Use:
$ build/domo

by default the app search for config file in /etc/domo.conf
in /examples/ you can found a example config file.
