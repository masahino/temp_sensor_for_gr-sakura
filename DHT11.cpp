/* 
  DHT11.h - Grove - Temperature and Humidity Sensor Pro driver library for GR-Sakura
  Modified 19 August,2012 for GR-sakura by Yoshinari Hayato
  http://japan.renesasrulz.com/mobile/Places/Application/Content/18?applicationId=3&applicationType=file
*/
 
/*
 * Version 1.0 April, 2012
 * Copyright 2012 Joinj
 * http://www.seeedstudio.com
 */

#include <rxduino.h>
#include "DHT11.h"


DHT11::DHT11(int pin)
{
    dht11_pin = pin;
}

void DHT11::begin()
{
  pinMode(dht11_pin,INPUT);
  digitalWrite(dht11_pin, HIGH);
}

int DHT11::getData()
{
/* error return 
     -1 ~  -8 : Failed to find start ( low level )
    -11 ~ -18 : Failed to find data ( high level )
*/
    byte i = 0;
    int result = 0;
    int itry;
  
    for(i=0; i< 8; i++){
        itry = 0;
        while(digitalRead(dht11_pin)){ // wait 0 to start
            delayMicroseconds(1);
            itry++;
            if (itry > 80){
                return -(i+1);
            }
        }
        itry = 0;
        while(!digitalRead(dht11_pin)){  // wait for 50us
            delayMicroseconds(1);
            itry ++;
            if (itry > 80){
                return -(i+11);
            }
        }
        delayMicroseconds(50);
 
        if(digitalRead(dht11_pin)){
            result |=(1<<(7-i));
        }
    }
    return result;
}

int DHT11::read(float *temp, float *humid)
{
/*
    return
         0         : Success
        -1         : CRC error
    -11 ~ -13      : Error in finding the data start 
    -101 ~ -118    : Failed in reading the 1st byte
    -201 ~ -218    : Failed in reading the 2nd byte
          :
    -501 ~ -518    : Failed in reading the 5th byte       
*/
    byte dht11_dat[5] = {0,0,0,0,0};
    byte dht11_check_sum = 0;
    int  i;
    int  itry;
    
    *temp  = -275.; 
    *humid = -100.; 
    
    digitalWrite(dht11_pin, HIGH);
    delay(250);
    pinMode(dht11_pin, OUTPUT);
    digitalWrite(dht11_pin,LOW);
    delay(18); /* minimum 18ms */
    digitalWrite(dht11_pin,HIGH);
    delayMicroseconds(20); /* 20 - 40 us */
    
    pinMode(dht11_pin,INPUT);
    delayMicroseconds(10); /* additional 10 us */
    
    itry = 0;
    while(digitalRead(dht11_pin) == HIGH){
        delayMicroseconds(1);
        itry++;
        if ( itry >= 30 ){
            return -11;
        }
    }
    delayMicroseconds(80);
    
    itry = 0;
    while( !digitalRead(dht11_pin) ){
        delay(1);
        itry++;
        if ( itry >= 20 ){
            return -12;
        }
    }
        
    delayMicroseconds(80);
    itry = 0;
    while( digitalRead(dht11_pin) ){
        delay(1);
        itry++;
        if ( itry >= 20 ){
            return -13;
        }
    }
    // now ready for data reception
    for (i=0; i<5; i++){
        itry = getData();
        if ( itry < 0 ){
            return -( itry - 100 * ( i + 1 ) );
        }
        dht11_dat[i] = itry;
    }
 
    pinMode(dht11_pin,OUTPUT);
    digitalWrite(dht11_pin,HIGH);
 
    dht11_check_sum = dht11_dat[0]+dht11_dat[1]+dht11_dat[2]+dht11_dat[3];
    // check check_sum
    if(dht11_dat[4]!= dht11_check_sum){
        return -1;
    }else{
        *humid=((float)(dht11_dat[0]*256+dht11_dat[1]))/10;
        *temp=((float)(dht11_dat[2]*256+dht11_dat[3]))/10;
        return 0;
    }
}

