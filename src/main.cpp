#include <Arduino.h>
#include <WiFiNINA.h> // wifi forbindelse
#include <Arduino_JSON.h> //json parsing
#include <Arduino_MKRIoTCarrier.h> //opla hardware
#include <wifiHq.h>
#include <displayFunc.h>
#include <gameControls.h>

MKRIoTCarrier carrier;
WiFiClient client;

String deviceId;

GameState state = DISCONNECTED;
GameState lastGameState = GAME_OVER;

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

  deviceId = wifi_getDeviceID();

  game_init (deviceId);
}

void loop() {
  delay(50);

  carrier.Buttons.update();

  if (state == DISCONNECTED) {

  }

  if (state == IN_QUEUE) {

  }

  if (state == IN_GAME) {

  }

  if (state == GAME_OVER) {

  }

  lastGameState = state;
}