/*
    Based on AdvancedWebServer example, by Majenko Technologies

    Based on: https://www.cnx-software.com/2016/03/22/getting-started-with-wemos-d1-mini-esp8266-board-dht-relay-shields/

  Depends on Adafruit DHT Arduino library
 * https://github.com/adafruit/DHT-sensor-library

*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "DHT.h"

const int led = 13;

const char *ssid = "yourSSID";
const char *password = "yourSSIDPassword";
const int port = 80;

// Define the webserver & port
ESP8266WebServer server ( port );

#define DHTPIN D4     // what pin we're connected to
 
// Uncomment whatever type you're using!
// #define DHTTYPE DHT11    // DHT 11
#define DHTTYPE DHT22       // DHT 22  (AM2302)
// #define DHTTYPE DHT21    // DHT 21 (AM2301)
 
// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
 
// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

// Define the DHT data struct
struct DHT_DATA {
  float hif;  // heat in ºF
  float hic;  // heat in ºC
  float humidity;
  float tempf;
  float tempc;
} data;

void setup ( void ) {
  pinMode ( led, OUTPUT );
  digitalWrite ( led, 0 );
  Serial.begin ( 115200 );
  WiFi.mode ( WIFI_STA );
  WiFi.begin ( ssid, password );
  Serial.println ( "" );

  // Wait for connection
  Serial.print("Waiting for connection ...");
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  Serial.println ( "" );
  Serial.print ( "Connected to " );
  Serial.println ( ssid );
  Serial.print ( "IP address: " );
  Serial.println ( WiFi.localIP() );

  if ( MDNS.begin ( "esp8266" ) ) {
    Serial.println ( "MDNS responder started" );
  }

  server.on ( "/", handleRoot );
  server.on ( "/inline", []() {
    server.send ( 200, "text/plain", "this works as well" );
  } );
  server.onNotFound ( handleNotFound );
  server.begin();!
  Serial.println ( "HTTP server started" );

  // Init the DHT Sensor
  dht.begin();
  Serial.println ( "DHT sensor initialized" );
}

void handleRoot() {
  digitalWrite ( led, 1 );
  char temp[500];
  
  // The main page reloads automactly every 5 seconds

  // Get the DHT data
  getDHTDataInfo();

  snprintf ( temp, 500,

  "<html>\
    <head>\
      <meta http-equiv='refresh' content='5'/>\
      <title>ESP8266 Demo</title>\
    </head>\
    <body>\
      <h1>Hello from ESP8266!</h1>\
      <hr />\
      <p>DHTs values</p>\
      <ul>\
        <li>Humidity: %d </li>\
        <li>Temperature: %d &deg;F</li>\
        <li>Temperature: %d &deg;C</li>\
        <li>Heat: %d &deg;F</li>\
        <li>Heat: %d &deg;C</li>\
      </ul>\
    </body>\
  </html>",
  
  data.humidity, data.tempf, data.tempc, data.hif, data.hic);
  
  server.send ( 200, "text/html", temp );
  digitalWrite ( led, 0 );
}

void handleNotFound() {
  digitalWrite ( led, 1 );
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
  digitalWrite ( led, 0 );
}

void loop ( void ) {
  server.handleClient();
}

void getDHTDataInfo() {

  // Warning: don't load this data within times less than 2s

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  // Set data struct
  data.hif = hif;
  data.hic = hic;
  data.humidity = h;
  data.tempc = t;
  data.tempf = f;

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");
}

