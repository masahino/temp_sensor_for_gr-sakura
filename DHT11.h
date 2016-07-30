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

class DHT11{
public:

    DHT11(int);
    void begin();
    int  getData();
    int  read(float *, float *);

    int dht11_pin;
};

