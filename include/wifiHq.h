#include <Arduino.h>

bool wifi_init(int timeoutMs);
String wifi_getDeviceID();
bool wifi_httpPost(const char* endpoint, String jsonBody, String& response);