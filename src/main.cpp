#include <Arduino.h>
#include <WiFiNINA.h> // wifi forbindelse
#include <Arduino_JSON.h> //json parsing
#include <Arduino_MKRIoTCarrier.h> //opla hardware
#include <wifiHq.h>
#include <displayFunc.h>
#include <gameControls.h>

void handleHeartbeat();

MKRIoTCarrier carrier;
WiFiClient client;

String deviceId;

GameState state = DISCONNECTED;
GameState lastGameState = GAME_OVER;
Direction dir;

int score = 0;

unsigned long lastHeartbeat = 0;

bool updateScreen = true;

void setup() {
  Serial.begin(9600);
  delay(1000);

  carrier.noCase();
  carrier.begin();

  carrier.display.setRotation(0);

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
    if (lastGameState != DISCONNECTED)
    {
      display_ScreenFill(ST7735_YELLOW);
      display_printCentered("DISCONNECTED :( ", 90, 2, ST7735_BLACK);
      display_printCentered("Press (04) to join", 120, 1, ST7735_BLACK);
    }
    
    if(carrier.Buttons.onTouchDown(TOUCH4)) {
      display_ScreenFill(ST77XX_ORANGE);
      display_printCentered("JOINING GAME...", 90, 2, ST7735_BLACK);
      display_printCentered("PLEASE WAIT STILL JOINING...", 120, 1, ST7735_BLACK);

      QueueResponse qr = game_joinQueue();

      Serial.println("Success: " + String(qr.success));
      Serial.println("State: " + String(qr.state));
      Serial.println("Direction: " + String(qr.direction));

      if (qr.success)
      {
        state = qr.state;
        dir = qr.direction;
      }
    }
    else {
      lastGameState = GAME_OVER;
    }
  }

  if (state == IN_QUEUE) {

    if(lastGameState != IN_QUEUE) {
      display_ScreenFill(ST7735_GREEN);
      display_printCentered("IN QUEUE WAITING...", 90, 2, ST7735_BLACK);
      display_printCentered("STILL WAITING...", 120, 2, ST7735_BLACK);
    }

    handleHeartbeat();
  }

  if (state == IN_GAME) {

    String dirString = "";

    if (dir == LEFT) {
      dirString = "LEFT";
    }
    else {
      dirString = "RIGHT";
    }

    if (updateScreen) {
      display_ScreenFill(ST7735_CYAN);
      display_printCentered("YOU ARE " + dirString, 90, 2, ST7735_BLACK);
      display_printCentered("SCORE: " + String(score), 120, 1, ST7735_BLACK);
      
      updateScreen = false;
    }

    if (carrier.Buttons.onTouchDown(TOUCH0) || carrier.Buttons.onTouchDown(TOUCH1) || carrier.Buttons.onTouchDown(TOUCH2) || carrier.Buttons.onTouchDown(TOUCH3) || carrier.Buttons.onTouchDown(TOUCH4)) {
      if(game_move(true)) {
        display_printCentered("YOU ARE MOVING DADDY", 150, 1, ST7735_BLACK);
      }
      else {
        display_printCentered("YOU ARE NOT MOVING DADDY", 150, 1, ST7735_BLACK);
      }

      delay(200);

      if(game_move(false)) {
        display_printCentered("YOU AINT MOVING DADDY", 150, 1, ST7735_BLACK);
      }
      else {
        display_printCentered("YOU AINT STOP MOVING DADDY", 150, 1, ST7735_BLACK);
      }

    }

    handleHeartbeat();
  }

  if (state == GAME_OVER) {

  }

  lastGameState = state;
}

void handleHeartbeat() {

  unsigned long now = millis();

  if (now - lastHeartbeat >= 5000) {

    HeartbeatResponse hr = game_sendHeartbeat();

    lastHeartbeat = now;

    if (hr.success)
    {
      state = hr.state;

      if (hr.score != score) {
        updateScreen = true;
      }

      score = hr.score;
    }
  }
}