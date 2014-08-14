#include <SPI.h>
#include <LiquidOLED.h>

LiquidOLED lcd(0x20,6);

void setup() 
{
  lcd.begin(16,2);
}

void loop(){
}