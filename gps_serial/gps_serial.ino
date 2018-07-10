/**********************
 *                    *    
 *   gps_serial.ino   *
 *                    *
 **********************   
 *   
 *  An example that shows how to read NMEA GPS data from a GY-GPS6MV2 device via serial software bus
 * 
 *  Based on the Leantec tutorial: https://leantec.es/blog/54_Tutorial-Arduino--Modulo-GPS-GPS6MV2.html
 *  And some more info at: https://naylampmechatronics.com/blog/18_tutorial-modulo-gps-con-arduino.html
 *  
 *  See my world at:
 *        _                          
 *       (_)___  _________ ___  _____
 *      / / __ \/ ___/ __ `__ \/ ___/
 *     / / /_/ / /  / / / / / / /__  
 *  __/ /\____/_/  /_/ /_/ /_/\___/  
 * /___/                             
 *
 *  @jormc
 *  http://www.jormc.es
 *  2018-10-07
 *
 *************************************************************************************************/

#include <SoftwareSerial.h>

// WEMOS (RX,TX) -> GPS (TX,RX)
SoftwareSerial gps(D4, D3);

char data=' ';

void setup()
{
  Serial.begin(115200);            
  gps.begin(9600); 
}

void loop()
{
  if(gps.available())
  {
    data=gps.read();
    Serial.print(data);
    delay (50);
  } else{
    Serial.printf("%d - GPS not available\n", millis());
    delay (1000);
  }
}
