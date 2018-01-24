#include "DHT.h"
 
#define DHTPIN D4         // Wemos shield connectos on pin 4
 
// Uncomment whatever type you're using!
//#define DHTTYPE DHT12     // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
 
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
  float tempc;
  float tempf;
} dhtData;

void setup() {
  // Init Serial Monitor at 115200
  Serial.begin(115200);
  Serial.println("Initializing DHT...");
 
  dht.begin();

  Serial.println("DHT successful initialized!");
}

void loop() {

  // Max refresh rate in DHT22 is 0,5Hz (2 seconds)
  // Wait a few seconds between measurements.
  delay(5000);
 
  dhtData.humidity = dht.readHumidity();
  dhtData.tempc = dht.readTemperature();
  dhtData.tempf = dht.readTemperature(true);
 
  if (isnan(dhtData.humidity) || isnan(dhtData.tempc) || isnan(dhtData.tempf)) {
    Serial.println("DHT read error!");
    return;
  }
 
  // Compute heat index in Fahrenheit (the default)
  dhtData.hif = dht.computeHeatIndex(dhtData.tempf, dhtData.humidity);
  
  // Compute heat index in Celsius (isFahreheit = false)
  dhtData.hic = dht.computeHeatIndex(dhtData.tempc, dhtData.humidity, false);

  // Print values to Serial Monitor
  print();
}

void print() {
  Serial.print("Humidity: ");
  Serial.print(dhtData.humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(dhtData.tempc);
  Serial.print(" *C ");
  Serial.print(dhtData.tempf);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(dhtData.hic);
  Serial.print(" *C ");
  Serial.print(dhtData.hif);
  Serial.println(" *F");
}

