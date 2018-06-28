#include <FS.h>

void setup() {

  Serial.begin ( 115200 );

  if (SPIFFS.begin()) {
    Serial.println("Filesystem successfully initialized");

    unsigned long start = millis();

    // Format the filesystem
    // format();

    // List all files in filesystem
    listFiles();

    unsigned long totalTime = millis() - start;
    
    Serial.printf("Total execution time: %dms", totalTime);

    SPIFFS.end();
        
  } else {
    Serial.println("Error while initializing filsystem");
  }
  
}

void listFiles() {
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    Serial.print("File: ");
    Serial.print(dir.fileName());
    File f = dir.openFile("r");
    Serial.print(", Size: ");
    Serial.print(f.size());
    Serial.println(" bytes");
  }
}

void format() {
  Serial.println("Start formatting the filesystem...");
  if (SPIFFS.format()) {
    Serial.println("Filesystem successfully formatted!");
  } else {
    Serial.println("Error while formatting filesystem");
  }
}

void loop() {

}