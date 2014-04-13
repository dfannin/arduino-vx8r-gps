arduino-vx8r-gps
================

Arduino code for buidling Yaesu VX-8R Handheld Transmitter-compatible GPS

# Overview

The Yaesu VX-8R HT for amateur radio VHF/UHF bands has a built-in APRS (Automatic Position Reporting System)  TNC (Terminal Node Controller) to operate as an APRS Beacon transmitter and receiver. To add position and velocity (GPS information) to the transmitted packet, an external Proprietary GPS module must be purchased from Yaesu and connected to HT.

The goal of this project was to create a compatbile GPS unit, made with open-source and generally available components, that could be used to proivde the position and velocity information for the Yaesu HT. The total cost of the project came in under $43USD, which included having to buy a proprietary cable for $22USB.

This GPS provides provides all the known GPS features used by the Yaesu, including position, altitude, speed, and direction. It also provides a GPS timestamp, which is used to automatically  set the time/date on the HT.  The GPS module and Arduino fit in a small candy tin (slightly larger than the Yaesu GPS unit),  and can be easily attached to the HT for portible operation.     

The code was designed to work with a specific GPS module (Ublox neo-6m), but should be easily adapted or extended to other NMEA-compatible GPS devices or required features. 

The unit was tested as a automobile  beacon transmitter, and provided acurate tracking information as reported by the http://aprs.fi website. 

This project builds from the work done by Taylor Meeks (http://www.lingnik.com/content/reverse-engineering-yaesu-vx-8dr-gps-interface).  


# Block Diagram
 ![BlockDiagram](https://raw.github.com/dfannin/arduino-vx8r-gps/master/arduino-vx8r-gps-fig01.png)

The GPS unit consists of three parts.  The GPS module provides NMEA messages via a two wire serial port (tx/rx). These messages are version 2.3 compliant.  The messages are sent to the Arduino, using a software serial port (AltSoftSerial), and are parsed, filtered and reformatted for the Yaesu.  The reformatted messages are sent to the Yaesu via the hardware serial port using a proprietary Yaesu cable (CT-M11 or equivilent), as the HT uses a 7 pin propreitary connector.  


# NMEA messages
The following NMEA messages are sent to the Yaesu:

* $GPGGA,hhmmss.sss,llll.llll,a,yyyyy.yyyy,a,x,xx,xx.x,xxxxx.x,M,xxxx.x,M,xxx.x,xxxx\*hh
* $GPGLL,3742.4413,N,12157.6537,W,175908.000,A,A\*4E
* $GPGSA,A,3,32,31,01,23,11,,,,,,,,4.7,2.2,4.1\*37
* $GPGSV,3,1,11,01,47,238,38,06,,,21,11,24,217,33,13,07,260,\*4D
* $GPRMC,hhmmss.sss,A,llll.llll,a,yyyyy.yyyy,a,xxxx.xx,xxx.xx,xxxxxx,,\*hh
* $GPVTG,,T,,M,0.46,N,0.8,K,A\*19
* $GPZDA,hhmmss.sss,xx,xx,xxxx,,\*hh


# Hardware

## NMEA-compatible GPS Module

## Arduino

## Yaesu HT

# Use  

# Extending/Modifiing Output 


