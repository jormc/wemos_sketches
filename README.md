# Wemos sketches collection

Some useful Wemos **D1 Mini Pro** sketches collection made with Arduino IDE.

Almost of them are suitables for the ESP866 chip too...

## Shields

* `dht22`: example sketch for the DHT22 sensor shield

## Wifi

* `mac` : shows the ESP8266 mac in the serial monitor
* `webserver`: very simple webserver made with the ESP8266
* `ap_sta_sequence`: start your ESP8266 into STA mode, and if timeout error, then starts the AP mode

## Filesystem

* `filesystem` : shows how to manage ESP8266 filesystem, format & list files (note, you must add your own files to `data` folder amd upload it to the devide previouly to upload this sketch)

## GPS

* `gps_serial` : shows how to obtain serial data from a NMEA GPS device, such like GY-GPS6MV2
* `gps_tinygps` : shows how to obtain serial data from a NMEA GPS device, such like GY-GPS6MV2, using the TinyGPS library (https://github.com/mikalhart/TinyGPS)

## Own ones

* `wemos_dht_mqtt_webserver` : (WORK IS IN PROGRESS) connect as AP/STA via ESP8266, read DHT22 sensor data, publish it over HTTP and MQTT
