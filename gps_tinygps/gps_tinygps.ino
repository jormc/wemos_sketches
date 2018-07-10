/**********************
 *                    *    
 *  gps_tinygps.ino   *
 *                    *
 **********************   
 *   
 *  An example that shows how to read NMEA GPS data from a GY-GPS6MV2 device via serial software bus
 *  using the great TinyGPS for interpreting the GPS data received
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
#include <TinyGPS.h>

// Declare the GPS object
TinyGPS gps;

// WEMOS (RX,TX) -> GPS (TX,RX)
SoftwareSerial serialgps(D4, D3);

// Data variables
int year;
byte month, day, hour, minute, second, hundredths;
unsigned long chars;
unsigned short sentences, failed_checksum;

void setup() {
    Serial.begin(115200);

    // Init GPS serial communication
    // It is always configured at 9600b by default
    serialgps.begin(9600);
        
    Serial.println("");
    Serial.println("GPS GY-GPS6MV2 Leantec");
    Serial.println(" ---Buscando senal--- ");
    Serial.println("");
}

void loop() {
    // If GPS is broadcasting data...
    while(serialgps.available()) {

        // Read serial data
        int c = serialgps.read(); 

        // Interpret data
        if(gps.encode(c)) {
            float latitude, longitude;
            gps.f_get_position(&latitude, &longitude);
            Serial.print("Latitud/Longitud: "); 
            Serial.print(latitude,5); 
            Serial.print(", "); 
            Serial.println(longitude,5);
            gps.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths);
            Serial.print("Fecha: "); Serial.print(day, DEC); Serial.print("/"); 
            Serial.print(month, DEC); Serial.print("/"); Serial.print(year);
            Serial.print(" Hora: "); Serial.print(hour, DEC); Serial.print(":"); 
            Serial.print(minute, DEC); Serial.print(":"); Serial.print(second, DEC); 
            Serial.print("."); Serial.println(hundredths, DEC);
            Serial.print("Altitud (metros): "); Serial.println(gps.f_altitude()); 
            Serial.print("Rumbo (grados): "); Serial.println(gps.f_course()); 
            Serial.print("Velocidad(kmph): "); Serial.println(gps.f_speed_kmph());
            Serial.print("Satelites: "); Serial.println(gps.satellites());
            Serial.println();
            gps.stats(&chars, &sentences, &failed_checksum);
        }
    }
}
