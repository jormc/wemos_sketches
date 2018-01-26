/****************************
 *                          *   
 *   ap_sta_sequence.ino    *
 *                          *
 ****************************   
 *   
 *  Try to start your ESP8266 into STA mode (don't forget to configure your WiFI SSID & PASSWORD).
 *  On unsucessful connection by timeout error ( waiting for more than 15s), then try to start in AP mode...
 *  
 *  Based on AdvancedWebServer example: https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/examples/AdvancedWebServer
 *  Based on WiFiAccessPoint example: https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi/examples/WiFiAccessPoint
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
 *  2018-26-01
 *
 *************************************************************************************************/
 
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// Remote WiFI SSID where to connect
// Leave them as erroneous to test STA to AP mode
const char* ssid      = "xxxxxxxx";
const char* password  = "xxxxxxxx";

// My devide SSID credentials
const char* apSSID      = "WEMOS_001";
const char* apPassword  = "1234567890";

// The WebServer, started at port 80
ESP8266WebServer server ( 80 );

const int LED = 13;
boolean connectionTimeout = false;

// The connection timeout in ms
#define SSID_CONNECT_TIMEOUT 15000

void setup ( void ) {
  initSerialMonitor();

  // Try to configure as STA
  // If connection timeout error, then connect as AP
  configureAsSTA();
  if (connectionTimeout) {
    configureAsAP();
  }
}

void loop ( void ) {
  // Do nothing for our example
  server.handleClient();
}

void initSerialMonitor() {
  Serial.begin(115200);
}

void configureAsAP(void) {
  delay(1000);

  Serial.println("Configuring access point mode...");

  // Set my SSID and my PASSWORD
  WiFi.softAP(apSSID, apPassword);

  // Get myIP
  IPAddress myIP = WiFi.softAPIP();

  // Print some own information
  Serial.print( "AP SSID: " );
  Serial.println(apSSID);
  Serial.print( "AP IP: " );
  Serial.println(myIP);

  // Attach the root page on server
  server.on("/", handleApRoot);
  server.begin();
  
  Serial.println( "Access Point started" );
}

void configureAsSTA() {
  delay(1000);
  
  pinMode ( LED, OUTPUT );
  digitalWrite ( LED, 0 );

  // Configure ESP8266 as STA
  WiFi.mode ( WIFI_STA );
  WiFi.begin ( ssid, password );

  Serial.println( "" );
  Serial.print ( "Starting in STA mode " );

  // Wait for connection
  unsigned long startTime = millis();
  unsigned long now = millis();
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 1000 );
    Serial.print ( "." );

    // Test timeout connection
    now = millis();
    if (now - startTime > SSID_CONNECT_TIMEOUT) {
      connectionTimeout = true;
      break;
    }
  }

  if (!connectionTimeout) {
    // Print the STA info
    Serial.println ( "" );
    Serial.print ( "Connected to " );
    Serial.println ( ssid );
    Serial.print ( "IP address: " );
    Serial.println ( WiFi.localIP() );
  
    if ( MDNS.begin ( "esp8266" ) ) {
      Serial.println ( "MDNS responder started" );
    }
  
    server.on ( "/", handleStaRoot );
    server.onNotFound ( handleNotFound );
    server.begin();
   
    Serial.println ( "HTTP server started" );
  } else {
    Serial.println( "" );
    Serial.print( "STA connection timeout! (waiting for more than " );
    Serial.print( SSID_CONNECT_TIMEOUT );
    Serial.println( "ms)" );
    Serial.println( "Are the configured ssid & password correct?" );
    Serial.println( "Sutting down STA mode configuration..." );
    Serial.println( "" );
  }
}

void handleApRoot() {
  server.send(200, "text/html", "<h1>You are connected to the AP</h1>");
}

void handleStaRoot() {
  server.send(200, "text/html", "<h1>You are connected to my server into the wifi network</h1>");
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

