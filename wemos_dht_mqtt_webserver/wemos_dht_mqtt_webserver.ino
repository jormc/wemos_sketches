/*************************************
 *                                   *   
 *   wemos_dht_mqtt_webserver.ino    *
 *                                   *
 *************************************   
 * 
 *  TRY: this is a test sketch where I want to try to integrate some relevant technologies and methods
 *  that will be implemented into my IoTSuite project.
 * 
 *  Feel free to test and edit it. Please tell me about yours developments about this.
 * 
 *  Integration:
 *  
 *      - ESP8266 WiFi Client / Webserver
 *      - DHT temperature & humidity sensor
 *      - Mqtt pub/sub
 *      - Http data JSON publicaton as REST style   
 * 
 *  Libraries:
 *
 *      - Arduino ESP8266 libraries: https://github.com/esp8266/Arduino/tree/master/libraries
 *      - Adafruit Sensor: https://github.com/adafruit/Adafruit_Sensor
 *      - Adafruit DHT Unified: https://github.com/adafruit/Adafruit_DHT_Unified/blob/master/DHT_U.h
 *      - Adafruit DHT: https://github.com/adafruit/DHT-sensor-library/blob/master/DHT.h
 *      - Arduino Client for MQTT: https://github.com/knolleary/pubsubclient
 *      - ArduinoJson: https://github.com/bblanchon/ArduinoJson
 * 
 *  TODO:
 * 
 *      - Reorganize & get better source code
 *      - Create own library
 *      - Make auto AP / STA selection 
 *      - Make possible update parameters via http pages > EEPROM writes
 * 
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
 *  2018-02-04
 *
 *************************************************************************************************/

// ESP8266 libraries
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>

// DHT libraries
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// MQTT
#include <PubSubClient.h>

// Other libraries
#include <ArduinoJson.h>

// AP Credentials
const char *ssid        = "Wemos001";
const char *password    = "1234567890";

// STA Credentials
const char *staSsid     = "xxxxxxxx";
const char *staPassword = "xxxxxxxx";

// Hardware config
const int LED     = LED_BUILTIN;

// ESP8266 Webserver object
ESP8266WebServer server(80);

// DHT shield config
#define DHTTYPE   DHT22
const int DHTPIN  = D4;
uint32_t delayMS;
sensor_t dhtSensor;
DHT_Unified dht(DHTPIN, DHTTYPE);

typedef struct {
  float value;
  char* unit;
} Sensor;

Sensor tempSensor = {0, "ºC"};
Sensor humSensor = {0, "%"};

// Other variables
unsigned long lastTime = 0;
DynamicJsonBuffer jsonBuffer(512);

// Your MQTT Brocker Credentials
#define mqtt_server "xxxxxxxx"
#define mqtt_user "xxxxxxxx"
#define mqtt_password "xxxxxxxx"
#define humidity_topic "sensor/humidity"
#define temperature_topic "sensor/temperature"

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

  delay(1000);

  // Setup serial
  Serial.begin(115200);

  // Setup Wemos as an AP
  // setupAP();

  // Setup Wemos as an AP
  setupSTA();

  // Setup the DHT shield
  setupDht();

  // Setup the mqtt client/server
  setupMqtt();
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  server.handleClient();

  if (millis() > lastTime + delayMS) {
    getDhtValues();
    publishMqtt();
    lastTime = millis();
  };

}

void setupAP() {
  // If password is not present, wifi will be open
  WiFi.softAP(ssid, password);

  // Get the AP IP
  IPAddress myIP = WiFi.softAPIP();

  Serial.println();
  Serial.println("Configuring access point...");

  Serial.print("AP IP address: ");
  Serial.println(myIP);

  // Handle requests
  server.on("/", handleApRoot);
  server.on("/config", handleApConfig);

  // Start the server
  server.begin();

  Serial.println("HTTP server started in AP mode");
}

void setupSTA() {

  pinMode (LED, OUTPUT);

  digitalWrite (LED, LOW);

  // COnfigure as STA mode
  WiFi.mode ( WIFI_STA );
  WiFi.begin (staSsid, staPassword );
  Serial.println ("");

  // Wait for connection
  Serial.print("Starting in STA mode");
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(staSsid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleStaRoot);
  server.on("/config", handleStaConfig);
  server.on("/data", handleGetSensorData);
  server.onNotFound(handleNotFound);
  server.begin();

  Serial.println ("HTTP server started in STA mode");
}

void setupDht() {

  Serial.println("Initializing DHT sensor...");

  // Initialize device.
  dht.begin();

  delay(1000);

  Serial.println("DHST sensor initialized successfully");

  // Print temperature sensor details.
  dht.temperature().getSensor(&dhtSensor);

  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(dhtSensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(dhtSensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(dhtSensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(dhtSensor.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(dhtSensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(dhtSensor.resolution); Serial.println(" *C");
  Serial.println("------------------------------------");

  // Print humidity sensor details.
  dht.humidity().getSensor(&dhtSensor);
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(dhtSensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(dhtSensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(dhtSensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(dhtSensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(dhtSensor.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(dhtSensor.resolution); Serial.println("%");
  Serial.println("------------------------------------");

  // Set delay between sensor readings based on sensor details.
  delayMS = (dhtSensor.min_delay / 1000);
}

void setupMqtt() {
  client.setServer(mqtt_server, 1883);
}

void publishMqtt() {
  client.publish(temperature_topic, String(tempSensor.value).c_str(), true);
  client.publish(humidity_topic, String(humSensor.value).c_str(), true);

  Serial.print("Published to mqtt server: ");
  Serial.print(temperature_topic);
  Serial.print(": ");
  Serial.print(tempSensor.value);
  Serial.print(" --- ");
  Serial.print(humidity_topic);
  Serial.print(": ");
  Serial.println(humSensor.value);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    // if (client.connect("ESP8266Client")) {
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void handleApRoot() {
  server.send(200, "text/html", "<h1>You are now connected to the AP</h1>");
}

void handleApConfig() {
  server.send(200, "text/html", "<h1>Wemos 001 administration page</h1><br />");
}

void handleStaRoot() {
  server.send(200, "text/html", "<h1>You are now connected to the STA</h1>");
}

void handleStaConfig() {
  server.send(200, "text/html", "<h1>Wemos 001 administration page</h1><br />");
}

void handleGetSensorData() {
  
  JsonObject& root = jsonBuffer.createObject();
  
  JsonObject& temp = jsonBuffer.createObject();
  temp["value"] = tempSensor.value;
  temp["unit"] = tempSensor.unit;
  
  JsonObject& hum = jsonBuffer.createObject();
  hum["value"] = humSensor.value;
  hum["unit"] = humSensor.unit;
  
  root["temperature"] = temp;
  root["humidity"] = hum;
  
  char jsonChar[512];
  root.printTo((char*)jsonChar, root.measureLength() + 1);
  server.send(200, "application/json", jsonChar);

}

void handleNotFound() {
  digitalWrite (LED, HIGH);

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

  digitalWrite (LED, LOW);
}

void getDhtValues() {

  // Delay between measurements.
  // delay(delayMS);

  // Get temperature event and print its value.
  sensors_event_t event;

  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    // Serial.println("Error reading humidity!");
    tempSensor.value = -100;
  } else {
    // Serial.print("Temperature: ");
    // Serial.println(event.temperature);
    tempSensor.value = event.temperature;
  }

  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    // Serial.println("Error reading humidity!");
    humSensor.value = -100;
  } else {
    // Serial.print("Humidity: ");
    // Serial.println(event.relative_humidity);
    humSensor.value = event.relative_humidity;
  }

}
