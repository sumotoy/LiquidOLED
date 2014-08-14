#include <SPI.h>
#include <LiquidOLED.h>

LiquidOLED lcd(0x20,6);

const int nRows = 2;      //number of rows on LCD
const int nColumns = 16;  //number of columns

const int length = nRows * nColumns;
char text[length+1];
char blanks[length+1];

void setup(void) {
  lcd.begin(nColumns,nRows);

}

void loop() {
  lcd.setCursor(0,0);
  char c = 'A';
  for (int i=0; i<length; i++) {
    text[i] = c++;
    blanks[i] = ' ';
    if (c > 'Z') c = 'A';
  }
  text[length] = 0;
  blanks[length] = 0;
  unsigned long startTime=millis();
  byte repetitions = 20;
  while (repetitions--) {
    lcd.setCursor(0,0);  // fill every screen pixel with text
    lcd.print(text);
    lcd.setCursor(0,0);  // then fill every pixel with blanks and repeat
    lcd.print(blanks);
  }
  unsigned long endTime = millis();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Benchmark ");
  lcd.print(nColumns, DEC);
  lcd.write('x');
  lcd.print(nRows, DEC);
  lcd.setCursor(0,1);
  lcd.print(endTime - startTime);
  lcd.print(" millisecs.");
  delay(1500);
  lcd.clear();
}

