#include <SPI.h>
#include <LiquidOLED.h>

LiquidOLED lcd(0x20,6);

void setup() 
{
  lcd.begin(16,2);
  lcd.print("Test:");
  delay(1000);
}

long tt = 0;

void loop(){
  
  lcd.setCursor(0,1);
  lcd.print("                   ");
  lcd.setCursor(0,1);
  lcd.print(tt,DEC);
  if (tt > 100000){
    tt = 0;
  } else {
    tt++;
  }
  delay(5);
}