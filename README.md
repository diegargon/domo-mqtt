# domo-mqtt
Domotica (Home automation)

I begin testing mqtt for home automation idea, here you can find some initial test files, and my initial programlisting

Will use:

A linux server: for:
	mosquitto (MQTT Broker)
A OrangePI (Raspberry pi clone) as:
	as a client,  open front door, camera, doorbell, IR/RADAR motion detector, etc
	Programming in C, using gcc, pahoo.mqtt.c (Async c library), WiringPi library, etc

Several ESP8266 (probably pragrammed with arduino), for small things like lights 

# requeriments

Linux Server (Or remote MQTT broker)
OrangePI and WiringPI, i have a orange pi H3 that work 
git clone https://github.com/zhaolei/WiringOP.git -b h3 
glib-2.0
pahoo.mqtt.c


# Use and compile

Compile:
$ make && make install

Will install a init.d script (/etc/init.d/domo) and /etc/domo.conf example file

by default the app search for config file in /etc/domo.conf
in /config/ you can found a example config file.
