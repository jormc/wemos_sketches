/****************
 *              *   
 *   mac.ino    *
 *              *
 ****************   
 *   
 *  Very simple sample sketch that shows the ESP3266 mac address
 *  in the Serial Monitor.
 * 
 *  See th original idea at: https://techtutorialsx.com/2017/04/09/esp8266-get-mac-address/
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
 *  2018-21-01
 *
 *************************************************************************************************/

#include <ESP8266WiFi.h>
 
void setup(){
 
   Serial.begin(115200);
   delay(500);
 
   Serial.println();
   Serial.print("MAC: ");
   Serial.println(WiFi.macAddress());
 
}
 
void loop(){}
