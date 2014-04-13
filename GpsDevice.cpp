// Copyright (c) 2014 David Fannin. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.
//
#include <Arduino.h>
#include "GpsDevice.h"

/*
 * GpsDevice
 * Class for managing GPS device input and output
 * provides NMEA message translation to required Yaesu output. 
 * version 1.0
 *
  */

GpsDevice::GpsDevice(AltSoftSerial &gdserial)
{
    _gdserial = gdserial;
    _init();
}

char GpsDevice::read(){

    char c;

    if ( isAvailable() || msg.length() >= GD_MAXLEN ) {
        _init();
    }

    if ( _gdserial.available() ) {
        c = _gdserial.read();
        switch(c) {
            case '$':
                sot = true ;
                ready = false ;
                origmsg = String(c);
                msg = String("");
                break;
            case '\n':
            case '\r':
                if(sot) {
                    sot = false;
                    ready = true;
                    filter();
                } else {
                    _init();
                }
                break ;
            default:
                if( sot ) {
                    origmsg += c ;
                }
                break ;
        }
    } else {
        c = NULL ;
    }
    return c;
}

bool GpsDevice::isAvailable(){
    return ready;
}

void GpsDevice::_init(){
    sot = false ;
    ready = false ;
    msg = String("");
    origmsg = String("");
}

void GpsDevice::begin(int speed){
    _gdserial.begin(speed);
}


byte GpsDevice::_atoi(char a) {
    if( int(a) >= 65 ) 
        return int(a) - 55;
     else
        return int(a) - 48;
}

void GpsDevice::write(char c){
    _gdserial.print(c);
}
bool GpsDevice::valid(){

    if( ! ready) return false ;
    
    int  plen = origmsg.length() - 3 ;
    pchksum=0;

    if ( plen < 1 ) return false ;

    psum[0]= origmsg[plen+1];
    psum[1]= origmsg[plen+2];

    pchksum =  ( _atoi(psum[0]) << 4 ) + _atoi(psum[1]) ;

    String pstr("");

    for(int i=1;i<plen;i++) {
        pstr +=  origmsg[i];
    }

    chksum = checksum(pstr);

    if ( chksum == pchksum) 
        return true ;
    else
        return false ;
}

byte GpsDevice::checksum(String str){
    char c ;
    byte cs = 0 ;
    int slen = str.length() ;
    for(int i=0;i<slen;i++) {
        c = (unsigned char) str[i];
        cs  ^= c;
    }
    return cs ;
}

String GpsDevice::schecksum(String str){
    String cs = String(checksum(str),HEX);
    cs.toUpperCase();
    if( cs.length() == 1) cs = "0" + cs ;
    return cs ;
}

String GpsDevice::_padzero(String str,int width){
    String nstr("");
    if( str[0] == '-') { 
        nstr += '-';
        for(int j=0;j< (width-1)-str.length();j++){
            nstr += '0' ;
        }
        nstr += str.substring(1,str.length());
    } else {
        for(int j=0;j<width-str.length();j++){
            nstr += '0' ;
        }
        nstr += str.substring(0,str.length());
    }
    return nstr; 

}

void GpsDevice::filter(){


    int pos;
    int pi=-1;
    // removes the checksum
    String parsemsg = origmsg.substring(0,origmsg.lastIndexOf('*')) ;

    // GPGAA messages
    if( origmsg.startsWith("$GPGGA,")){

        do {

            pos = parsemsg.indexOf(',');
            pi++;

            if(pos != -1) {
                switch(pi) {
                    case 0:
                        msg = parsemsg.substring(0,pos) + ',' ;
                        break;
                    case 8:
                        msg += _padzero(parsemsg.substring(0,pos),4) + ','; 
                        break;
                    case 9:
                        msg += _padzero(parsemsg.substring(0,pos),7) + ','; 
                        break;
                    case 11:
                        msg += _padzero(parsemsg.substring(0,pos),6) + ','; 
                        break;
                    case 13:
                        msg += "000.0,";
                        break;
                    case 14:
                        msg += "0000" ;
                        break;
                    default:
                        msg += parsemsg.substring(0,pos) + ',' ;
                        break;
                }

                parsemsg = parsemsg.substring(pos+1,parsemsg.length());

            } else {
                // handles case 14
                msg += "0000" ;
            }
        
        } while(pos >=0) ; // end do while
        // add checksum
        msg += "*" + schecksum(msg.substring(1,msg.length())) + "\r" ;
        return;
    } // end GPGGA

    // GPRMC messages
    if( origmsg.startsWith("$GPRMC,")){

        do {

            pos = parsemsg.indexOf(',');
            pi++;

            if(pos != -1) {
                switch(pi) {
                    case 0:
                        msg = parsemsg.substring(0,pos) + ',' ;
                        break;
                    case 7:
                        msg += _padzero(parsemsg.substring(0,pos),7) + ','; 
                        break;
                    case 8:
                    case 9:
                        msg += _padzero(parsemsg.substring(0,pos),6) + ','; 
                        break;
                    default:
                        msg += parsemsg.substring(0,pos) + ',' ;
                        break;
                }

                parsemsg = parsemsg.substring(pos+1,parsemsg.length());

            } else {
                // handles case 14
                msg += "," ;
            }
        
        } while(pos >=0) ; // end do while
        // add checksum
        msg += "*" + schecksum(msg.substring(1,msg.length())) + "\r" ;
        return;
    } // end GPRMC
    // GPTXT messages
    if( origmsg.startsWith("$GPTXT,")){
        msg = "";
        return;
    } // end GPRMC

    msg = origmsg + "\r" ;
}
