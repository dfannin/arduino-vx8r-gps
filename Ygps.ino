// Copyright (c) 2014 David Fannin. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.
//
// Version 1.0
#include <AltSoftSerial.h>
#include "GpsDevice.h"

AltSoftSerial gpsserial;

GpsDevice gps(gpsserial);

void setup() {
    Serial.begin(9600);
    gps.begin(9600);
}

void loop(){

    char c ;
    
    gps.read();  // read a character from gps

    // if parsed, filtered message is available, send it to the Yaesu
    if(gps.isAvailable()) {
        // make sure the checksum is valid
        if (gps.valid() ) {
            Serial.println(gps.msg);
        }
    }

    // send yaesu output to the gps
    if(Serial.available()) {
        c = Serial.read();
        gps.write(c);
    }

}
