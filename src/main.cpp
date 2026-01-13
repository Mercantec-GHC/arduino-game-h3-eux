#include <Arduino.h>
#include <WiFiNINA.h> // wifi forbindelse
#include <Arduino_JSON.h> //json parsing
#include <Arduino_MKRIoTCarrier.h> //opla hardware
#include <wifiHq.h>
#include <displayFunc.h>

MKRIoTCarrier carrier;
WiFiClient client;


void setup() {
  Serial.begin(9600);
  delay(1000);

  carrier.noCase();
  carrier.begin();

  display_Init(carrier);

  if (!wifi_init(5000)) {

    Serial.println("Failed to connect to WiFi");
    
    while(1);
  }


}

void loop() {
  // put your main code here, to run repeatedly:
}