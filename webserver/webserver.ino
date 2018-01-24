/**********************
 *                    *    
 *   webserver.ino    *
 *                    *
 **********************   
 *   
 *  An example that shows how build a simple webserver made with the ESP8266 and Wemos 
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
 *  2018-24-01
 *
 *************************************************************************************************/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char *ssid      = "xxxxxxxx";
const char *password  = "xxxxxxxx";
ESP8266WebServer server ( 80 );

const int LED = 13;

void setup() {

  // Turn off the LED
  pinMode ( LED, OUTPUT );
  digitalWrite ( LED, 0 );

  // Start Serial monitor at 115200b
  Serial.begin ( 115200 );

  Serial.print( "Starting web server ..." );

  // Set WiFi STA mode
  WiFi.mode ( WIFI_STA );

  // Begin WiFi session with credentials
  WiFi.begin ( ssid, password );
  Serial.println ( "" );

  // Wait for connection
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
  server.begin();
  Serial.println ( "HTTP server started" );

}

void loop ( void ) {
  server.handleClient();
}

void handleRoot() {

  char temp[400];
  sprintf ( temp, "HelloWorld!!!");
  server.send ( 200, "text/html", temp );
  
}

void handleNotFound() {
  digitalWrite ( LED, 1 );
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
  digitalWrite ( LED, 0 );
}
