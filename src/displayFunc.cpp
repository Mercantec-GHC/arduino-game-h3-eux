#include <displayFunc.h>
 
Adafruit_ST7789* display;
int textSize = 1;
 
const int SCREEN_W = 240;
const int SCREEN_H = 240;
 
void display_Init(MKRIoTCarrier& carrier)
{
    display = &carrier.display;
}
 
void display_ScreenFill(int color)
{
    display->fillScreen(color);
}
 
void display_TextSize(int size)
{
    display->setTextSize(size);
    textSize = size;
}
 
void display_TextColor(int color)
{
    display->setTextColor(color);
}
 
void display_Print(String text, int x, int y, int size)
{
    if (size != 100)
    {
        display->setTextSize(size);
    }
 
    if (x != 255 && y != 255)
    {
        display->setCursor(x, y);
    }
    display->print(text);
 
    display->setTextSize(textSize);
}
 
void display_PrintLn(String text, int x, int y, int size)
{
    if (size != 100)
    {
        display->setTextSize(size);
    }
 
    if (x != 255 && y != 255)
    {
        display->setCursor(x, y);
    }    
    display->println(text);
 
    display->setTextSize(textSize);
}
 
void display_printCentered(String text, int y, int size, uint16_t color)
{
    display->setTextSize(size);
    display->setTextColor(color);
 
    int charWidth = 6 * size;
    int textWidth = text.length() * charWidth;
 
    int x = (SCREEN_W - textWidth) / 2;
 
    display->setCursor(x, y);
    display->print(text);
}