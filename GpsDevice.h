// Copyright (c) 2014 David Fannin. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.
//
// Version 1.0
//
#ifndef GpsDevice_h
#define GpsDevice_h

#include <Arduino.h>
#include <AltSoftSerial.h>

// max length of input GPS message 
#define GD_MAXLEN 120


class GpsDevice
{
    public:
        GpsDevice(AltSoftSerial &gdserial);
        char read();
        bool isAvailable();
        void filter();
        void write(char c);
        void begin(int speed);
        bool valid();
        byte checksum(String str);
        String schecksum(String str);
        byte chksum;
        byte pchksum;
        char psum[2];
        String msg;
        String origmsg;
    private:
        bool sot;
        bool ready;
        void _init();
        byte _atoi(char a);
        String _padzero(String str,int width);
    protected:
        AltSoftSerial _gdserial;
};

#endif
