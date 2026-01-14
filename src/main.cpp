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
Direction dir = MOVE_LEFT;

int score = 0;

unsigned long lastHeartbeat = 0;

bool updateScreen = true;

bool moving = false;

uint32_t color_red = 0xFF0000;
uint32_t color_green = 0x00FF00;
uint32_t color_orange = 0xFFA500;

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
      carrier.leds.clear();
      carrier.leds.show();

      display_ScreenFill(ST7735_YELLOW);
      display_printCentered("ID: " + deviceId, 70, 1, ST7735_BLACK);
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
      display_ScreenFill(ST7735_ORANGE);
      display_printCentered("IN QUEUE!", 90, 2, ST7735_BLACK);
      display_printCentered("PRESS (02) TO LEAVE", 120, 2, ST7735_BLACK);

      for (uint8_t i = 0; i < 5; i++) {
        carrier.leds.setPixelColor(i, color_orange);
      }
      carrier.leds.show();
    }

     if (carrier.Buttons.onTouchDown(TOUCH2)) {
        state = DISCONNECTED;
        return;
    }

    handleHeartbeat();
  }

  if (state == IN_GAME) {

    if (carrier.Buttons.onTouchDown(TOUCH2)) {
      state = DISCONNECTED;
      return;
    }

    handleHeartbeat();

    String dirString = "";

    if (dir == MOVE_LEFT) {
      dirString = "LEFT";
    }
    else {
      dirString = "RIGHT";
    }


    touchButtons moveButton = TOUCH0;
    touchButtons stopButton = TOUCH4;

    if (dir == MOVE_RIGHT) {
      moveButton = TOUCH4;
      stopButton = TOUCH0;
    } 
    else {
      moveButton = TOUCH0;
      stopButton = TOUCH4;
    }
    
    if (!moving) {
      if (carrier.Buttons.onTouchDown(moveButton))
      {
        if (game_move(true)) {
          for (uint8_t i = 0; i < 5; i++) {
            carrier.leds.setPixelColor(i, color_green);
          }
          carrier.leds.show();

          Serial.println("MOVING");
          moving = true;
        }
        else {
          Serial.println("FAILED TO MOVE");
        }
      }
    } 
    else {
      if (carrier.Buttons.onTouchDown(stopButton)) {
        if (game_move(false)) {
          for (uint8_t i = 0; i < 5; i++) {
            carrier.leds.setPixelColor(i, color_red);
          }
          carrier.leds.show();

          moving = false;
          updateScreen = true;
        }
      }
    }
    
    if (updateScreen) {
      display_ScreenFill(ST7735_CYAN);
      display_printCentered(dirString, 60, 2, ST7735_BLACK);
      display_printCentered("MOVE: " + String(moveButton), 90, 2, ST7735_BLACK);
      display_printCentered("STOP: " + String(stopButton), 120, 2, ST7735_BLACK);
      display_printCentered("SCORE: " + String(score), 150, 1, ST7735_BLACK);
      
      updateScreen = false;
    }
  }

  if (state == GAME_OVER) {
    if (lastGameState != GAME_OVER)
    {
      carrier.leds.clear();
      carrier.leds.show();

      display_ScreenFill(ST7735_BLACK);
      display_printCentered("GAME OVER MAN!", 90, 2, ST7735_RED);
      display_printCentered("SCORE: " + String(score), 120, 2, ST7735_RED);
      display_printCentered("PRESS (02) TO QUIT", 150, 1, ST7735_RED);
    }

    if (carrier.Buttons.onTouchDown(TOUCH2)) {
      state = DISCONNECTED;
      return;
    }
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