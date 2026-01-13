#include <wifiHq.h>
#include <WiFiNINA.h>
#include <Arduino_MKRIoTCarrier.h>
#include <displayFunc.h>
#include <config.h>

extern MKRIoTCarrier carrier;
extern WiFiClient client;

String deviceId;

bool wifi_init (int timeoutMs) {
    uint8_t retries = 3;
    bool connected = false;

    for (uint8_t i = 0; i < retries; i++) {
        Serial.println("Connection to WIFI...");
        Serial.print("SSID: ");
        Serial.println(WIFI_SSID);
        Serial.print("PASS: ");
        Serial.println(WIFI_PASS);

        display_ScreenFill(ST7735_BLACK);
        display_TextSize(1);
        display_PrintLn("Connecting to WIFI", 50, 60);
        display_Print("SSID: ", 50, 90);
        display_PrintLn(WIFI_SSID);
        display_Print("Pass: ", 50, 100);
        display_PrintLn(WIFI_PASS);

        WiFi.begin(WIFI_SSID, WIFI_PASS);
        int startMs = millis();

        while (millis() - startMs <= timeoutMs){
            delay(timeoutMs / 43);
            Serial.print(".");
            display_Print(".");

            if (WiFi.status() == WL_CONNECTED) {
                connected = true;
                break;
            }
        }
        if (connected) {
            break;
        }
        else{
            Serial.println("\nFailed to connect to WiFi...");
            display_PrintLn("\nFailed to connect to WiFi...", 50, 120);
            display_PrintLn("Retrying" + String(i + 1) + "/" + String(retries) + "...", 50, 140);
            delay(750);
        }
    }

    Serial.println("WIFI connection: OK");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    display_PrintLn("WIFI Connection: OK", 50, 110);
    display_PrintLn(WiFi.localIP().toString());

    return true;
}

String wifi_getDeviceID() {

    byte mac[6];
    WiFi.macAddress(mac);
    char buf[20];
    snprintf(buf, sizeof(buf), "OPLA_%02X%02X%02X", mac[3], mac[4], mac[5]);
    deviceId = String(buf);

    Serial.print("Device ID: ");
    Serial.println(deviceId);

    return deviceId;
}

String readResponseBody() {
    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r" || line.length() == 0) break;

    }
    return client.readString();
}

bool httpPost(const char* endpoint, String jsonBody, String& response) {
    Serial.print ("POST");
    Serial.println(endpoint);

    if (!client.connect(SERVER_IP, SERVER_PORT)){
        Serial.println(" -> Connection failed");
        return false;
    }

    client.print("POST");
    client.print("endpoint");
    client.println("HTTP/1.1");
    client.print("Host: ");
    client.println(SERVER_IP);
    client.println("Conetent-Type: application/json");
    client.print("Content-Length");
    client.println(jsonBody.length());
    client.println("Connection: close");
    client.println();
    client.print(jsonBody);

    String statusLine = client.readStringUntil('\n');
    response = readResponseBody();
    client.stop();

    int jsonStart = response.indexOf('{');
    if (jsonStart >= 0) {
        response = response.substring(jsonStart);
    }

    bool success = statusLine.indexOf("200") > 0;
    Serial.print(" -> ");
    Serial.println(success ? "OK" : "FAILED");

    return success;

}