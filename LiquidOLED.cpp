#include "LiquidOLED.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
#include <../SPI/SPI.h>//this chip needs SPI



//instance
LiquidOLED::LiquidOLED(uint8_t address,uint8_t cspin){
	//dati per il GPIO
	_address = address;
	_cs_Pin = cspin;
	_readCmd =  (_address << 1) | 1;
	_writeCmd = _address << 1;
	_lcdControlPins = 0b00000000;
	_lcdDataPins = 0b00000000;
	//inizializza i dati dell'LCD
	_x = 0;
	_y = 0;
	_SPItransactionSpeed = 0;
}

LiquidOLED::LiquidOLED(uint8_t address,uint8_t cspin,uint32_t spispeed){
	//dati per il GPIO
	_address = address;
	_cs_Pin = cspin;
	_readCmd =  (_address << 1) | 1;
	_writeCmd = _address << 1;
	_lcdControlPins = 0b00000000;
	_lcdDataPins = 0b00000000;
	//inizializza i dati dell'LCD
	_x = 0;
	_y = 0;
	#if defined (SPI_HAS_TRANSACTION)
	_SPItransactionSpeed = spispeed;
	#else
	_SPItransactionSpeed = 0;
	#endif
}

//----------------------------------------GPIO
void LiquidOLED::gpioStartSend(bool mode){
	#if defined (SPI_HAS_TRANSACTION)
	if (_SPItransactionSpeed > 0)
	SPI.beginTransaction(SPISettings(_SPItransactionSpeed, MSBFIRST, SPI_MODE0));
	#endif
#if defined(__MK20DX128__) || defined(__MK20DX256__)
	digitalWriteFast(_cs_Pin, LOW);
#else
	digitalWrite(_cs_Pin, LOW);
#endif
	mode == 1 ? SPI.transfer(_readCmd) : SPI.transfer(_writeCmd);
}

void LiquidOLED::gpioEndSend(){
#if defined(__MK20DX128__) || defined(__MK20DX256__)
	digitalWriteFast(_cs_Pin, HIGH);
#else
	digitalWrite(_cs_Pin, HIGH);
#endif
	#if defined (SPI_HAS_TRANSACTION)
	if (_SPItransactionSpeed > 0)
	 SPI.endTransaction();
	#endif
}

void LiquidOLED::gpioWriteByte(byte reg, byte data){
	gpioStartSend(0);
	SPI.transfer(reg);
	SPI.transfer(data);
	gpioEndSend();
}

void LiquidOLED::gpioWriteWord(byte reg, uint16_t data){
	gpioStartSend(0);
	SPI.transfer(reg);
	SPI.transfer((byte)(data & 0x00FF));
	SPI.transfer((byte)(data >> 8));
	gpioEndSend();
}

void LiquidOLED::gpioWriteBytes(byte dati, byte controllo){
	gpioStartSend(0);
	SPI.transfer(GPIO);
	SPI.transfer(dati);//portA
	SPI.transfer(controllo);//portB
	gpioEndSend();
}

void LiquidOLED::dwrite(uint8_t pat, uint8_t x, uint8_t y) {
    //command(0x80 + (x&0x7f)); // x position
    //command(0x40 + (y&0x01)); //y position
    command(LCD_SETDDRAMADDR | x);
    command(LCD_SETCGRAMADDR | y);
    write(pat);
}

//---------------------------------------
void LiquidOLED::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
	uint8_t i;
	_lcd_cols = cols;
	_lcd_lines = lines;
	//inizializza SPI
	SPI.begin();
	#if defined (SPI_HAS_TRANSACTION)
	if (_SPItransactionSpeed < 1){
		SPI.setClockDivider(SPI_CLOCK_DIV4); // 4 MHz (half speed)
		SPI.setBitOrder(MSBFIRST);
		SPI.setDataMode(SPI_MODE0);
	}
	#else
	SPI.setClockDivider(SPI_CLOCK_DIV4); // 4 MHz (half speed)
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
	#endif
	pinMode(_cs_Pin, OUTPUT);
	digitalWrite(_cs_Pin, HIGH);
	delay(100);
	//inizializza GPIO
	gpioWriteByte(IOCON,0b00101000);//setta HAEN e parametri base del GPIO
	gpioWriteWord(IODIR,0x0000);//0xFFFF(IN) - 0x0000(out)//tutte uscite!
	
	gpioWriteBytes(_lcdDataPins,_lcdControlPins);//aggiorna il GPIO
// On power up, the display is initilaized as:
// 1. Display clear
// 2. Function set:
//    DL="1": 8-bit interface data
//    N="0": 1-line display
//    F="0": 5 x 8 dot character font
// 3. Power turn off
//    PWR=”0”
// 4. Display on/off control: D="0": Display off C="0": Cursor off B="0": Blinking off
// 5. Entry mode set
//    I/D="1": Increment by 1
//    S="0": No shift
// 6. Cursor/Display shift/Mode / Pwr
//    S/C=”0”, R/L=”1”: Shifts cursor position to the right
//    G/C=”0”: Character mode
//    Pwr=”1”: Internal DCDC power on
//
	if (LCDBITMODE < 8){//4bit
		//_displayfunction = LCD_FUNCTIONSET | LCD_4BITMODE;
		//_displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
		_displayfunction = LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS | LCD_EUROPEAN_I;
		_displaycontrol  = LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
		_displaymode     = LCD_MODEPOWER;
		
		for (i=0;i<5;i++){
			command(_displayfunction);//8bit/2Lines/5x8dots/default table 0b00111000
			delayMicroseconds(5000);
		}
		write4bits(0x03);  					//00000011
		delayMicroseconds(5000);
		write4bits(0x02);					//00000010
		delayMicroseconds(5000);
		write4bits(0x02);					//00000010
		delayMicroseconds(5000);
		write4bits(0x08);					//00001000
   
		delayMicroseconds(5000);
  
		command(0x08);	// Function Set		//00001000 (D:off,C:off,B:off)
		delayMicroseconds(5000);
		command(LCD_CLEARDISPLAY);
		delayMicroseconds(5000);
		command(0x06);	// Entry Mode Set	//00000110 (incr.ON/Shift disabled)
		delayMicroseconds(5000);
		command(LCD_RETURNHOME);
		delayMicroseconds(5000);
		command(0x0C);	// Function Set     //00001100 (D:on,C:off,B:off)
		delayMicroseconds(5000);
	} else {//8bit
		
		_displayfunction = LCD_FUNCTIONSET | LCD_8BITMODE | LCD_2LINE | LCD_5x8DOTS | LCD_EUROPEAN_I;
		_displaycontrol  = LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
		_displaymode     = LCD_MODEPOWER | LCD_TEXTMODE | LCD_POWERON;
		
		for (i=0;i<4;i++){
			command(_displayfunction);//8bit/2Lines/5x8dots/default table 0b00111000
			delay(1);
		}
		//0b0001
		//      S/C R/L 00
		//      GC  PWR 11
		command(_displaycontrol);
		delay(1);
		command(LCD_ENTRYMODESET | LCD_INCREMENTBIT);//shift?
		//command(0b00010100);
		delay(1);

		command(_displaymode);//starts in textMode! LCD_SETCHARCTERMODE
		delay(1);
		command(LCD_RETURNHOME);
		delay(1);
		command(LCD_CLEARDISPLAY);
		delay(1);
		
		/*
		_displayfunction = 0x38;
		_displaycontrol = 0x0C;
		for (i=0;i<4;i++){
			command(_displayfunction);//8bit/2Lines/5x8dots/default table 0b00111000
			delay(1);
		}
		command(_displaycontrol); //Display ON/OFF control,Display ON,Cursor&Blink OFF 
		delay(1); 
		command(0x06); //Entry Mode Set, address increment & Shift off 
		delay(1); 
		command(LCD_SETCHARCTERMODE); //Graphic mode 
		delay(1); 
		command(LCD_RETURNHOME); //Return Home 
		delay(1); 
		command(LCD_CLEARDISPLAY); //Clear Display 
		delay(1); 
		*/
	}
	_currentMode = 0;//starts in textMode!
}

void LiquidOLED::setTextMode(bool mode) {
	if (_currentMode == 1) {
		//do stuff here to change state (TODO)
		_displaymode     = LCD_MODEPOWER | LCD_TEXTMODE | LCD_POWERON;
		command(_displaymode);//starts in textMode! LCD_SETCHARCTERMODE
		delay(1);
		_currentMode = 0;
	}	
}

void LiquidOLED::setGraphicMode(bool mode) {
	if (_currentMode == 0) {
		_displaymode     = LCD_MODEPOWER | LCD_GRAPHICMODE | LCD_POWERON;
		command(_displaymode);//starts in textMode! LCD_SETCHARCTERMODE
		delay(1);
		_currentMode = 1;
	}
}

bool LiquidOLED::getTextMode(void) {
	if (_currentMode) return false;
	return true;
}

bool LiquidOLED::getGraphicMode(void) {
	if (_currentMode) return true;
	return false;
}

void LiquidOLED::send(uint8_t value, uint8_t mode) {
	if (mode == HIGH){
		bitSet(_lcdControlPins,LCDPIN_RS);
	} else {
		bitClear(_lcdControlPins,LCDPIN_RS);
	}
	bitClear(_lcdControlPins,LCDPIN_RW);
	//gpioWriteBytes(value,_lcdControlPins);//aggiorna il GPIO
	if (LCDBITMODE < 8){//4bit
		write4bits(value>>4);
		write4bits(value);
	} else {
		write8bits(value);
	}
}

inline void LiquidOLED::command(uint8_t value) {
  send(value, LOW);
  //in normal life this will wait until chip ready
}

inline size_t LiquidOLED::write(uint8_t value) {
  send(value, HIGH);
  //in normal life this will wait until chip ready
  return 1;
}

void LiquidOLED::pulseEnable(uint8_t value) {
	bitClear(_lcdControlPins,LCDPIN_EN);
	gpioWriteBytes(value,_lcdControlPins);//aggiorna il GPIO
	delayMicroseconds(1);//40
	bitSet(_lcdControlPins,LCDPIN_EN);
	gpioWriteBytes(value,_lcdControlPins);//aggiorna il GPIO
	delayMicroseconds(1); //40
	bitClear(_lcdControlPins,LCDPIN_EN);
	gpioWriteBytes(value,_lcdControlPins);//aggiorna il GPIO
	delayMicroseconds(1);//40
}

void LiquidOLED::write4bits(uint8_t value) {
	uint8_t i;
	for (i = 0; i < 4; i++) {
		if ((value >> i) & 0x01){
			bitSet(_lcdDataPins,LCDDATAPINS[4+i]);
		} else {
			bitClear(_lcdDataPins,LCDDATAPINS[4+i]);
		}
	}
	pulseEnable(_lcdDataPins);
}

void LiquidOLED::write8bits(uint8_t value) {
	uint8_t i;
	for (i = 0; i < 8; i++) {
		if ((value >> i) & 0x01){
			bitSet(_lcdDataPins,LCDDATAPINS[i]);
		} else {
			bitClear(_lcdDataPins,LCDDATAPINS[i]);
		}
	}
	pulseEnable(_lcdDataPins);
}

//----------------------------------------------

void LiquidOLED::clear(){
	command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
	delay(7);
}

void LiquidOLED::home(){
	command(LCD_RETURNHOME);  // set cursor position to zero
	delayMicroseconds(1);
}

void LiquidOLED::setCursor(uint8_t col, uint8_t row){
	if (_currentMode){
		//if (col > 100) col = 100;
		//if (row > 1) row = 1;
		command(LCD_SETDDRAMADDR | col);
		command(LCD_SETCGRAMADDR | row);
	} else {
		int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
		if (row >= _lcd_lines) row = _lcd_lines-1;    // we count rows starting w/0
		command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
	}
}

void LiquidOLED::noDisplay() {
	_displaycontrol &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidOLED::display() {
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidOLED::noCursor() {
	_displaycontrol &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidOLED::cursor() {
	_displaycontrol |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidOLED::noBlink() {
	_displaycontrol &= ~LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidOLED::blink() {
	_displaycontrol |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidOLED::scrollDisplayLeft(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void LiquidOLED::scrollDisplayRight(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void LiquidOLED::leftToRight(void) {
	_entrymode |= LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _entrymode);
}

void LiquidOLED::rightToLeft(void) {
	_entrymode &= ~LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _entrymode);
}

void LiquidOLED::autoscroll(void) {
	_entrymode |= LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _entrymode);
}

void LiquidOLED::noAutoscroll(void) {
	_entrymode &= ~LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _entrymode);
}

void LiquidOLED::createChar(uint8_t location, uint8_t charmap[]) {
	uint8_t i;
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));
	for (i=0; i<8; i++) {
		write(charmap[i]);
	}
}