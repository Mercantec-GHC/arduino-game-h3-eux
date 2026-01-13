#include <Arduino.h>
#include <Arduino_MKRIoTCarrier.h>
 
void display_Init(MKRIoTCarrier& carrier);
void display_ScreenFill(int color);
void display_TextSize(int size);
void display_TextColor(int color);
void display_Print(String text, int x = 255, int y = 255, int size = 100);
void display_PrintLn(String text, int x = 255, int y = 255, int size = 100);
void display_printCentered(String text, int y, int size, uint16_t color);