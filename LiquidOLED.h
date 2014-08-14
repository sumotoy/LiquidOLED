/*
A driver for winstar WEH001602A Text OLED Module 16*2 that uses 3 wires.

Attention! 
---> This driver uses a GPIO Chip called MCP23S17! <---
---> Alpha development stage! Text working 100%, still develop graphic mode <---

Version:0.1a1

Preface:
This display can be easily buy in Ebay and from Adafruit that provided a terrible
library for that, incomplete and full of bugs. Looking around doesn't help me a lot,
no working libraries (or 'Almost' working ones) so I decided to learn more about this
controller. Here's my first attempt to create a working library fully compatible
with LiquidCrystal commands with the ability to switch in graphic mode (when it will work).
I'm using a GPIO to save pins because in graphic mode I need access to 8 bit data, this
will result in 11 pin at minimum! Too much for a cheap and small MCU so what about usea GPIO
that let you save almost all pins (if you decide to use HAEN feature and compatible GPIO's)?
WEH001 controllers are slow, there's no need to coupled directly to MCU to speed up it.
Library can drive Oled in 4 or 8 bit mode, it's in a early development stage so things will
probably change but text mode it's 100% functional so it can be used.


Connections:
MCP23S17 --> WEH001602A display
Can work at 3V3 or 5V.

	RW	-->	  IOB-0 [|  U   |] IOA-7	-->	D7
	RS  -->	  IOB-1 [| 	 	|] IOA-6	-->	D6
	E   -->	  IOB-2 [| 	 	|] IOA-5	-->	D5
			  IOB-3 [| 	 	|] IOA-4	-->	D4
			  IOB-4 [| 	 	|] IOA-3	-->	D3
			  IOB-5 [| 	 	|] IOA-2	-->	D2
			  IOB-6 [| 	 	|] IOA-1	-->	D1
			  IOB-7 [| 	 	|] IOA-0	-->	D0
			  ++++  [| 	 	|] [INT-A] not used
			  GND   [| 	 	|] [INT-B] not used
			  CS 	[| 	 	|] RST (connect to +++)
			  SCK   [| 	 	|] A2	*see addressing
			  MOSI  [| 	 	|] A1   *see addressing
		[not used]  [|______|] A0	*see addressing
		
	*Addressing (see MCP23s17 datasheet)
	Ground all 3 will result in 0x20	
---------------------------------------

*/

#ifndef LiquidOLED__h
#define LiquidOLED__h

#include <inttypes.h>
#include "Arduino.h"
#include "Print.h"
#include <../SPI/SPI.h>//this chip needs SPI


#define LCDBITMODE				8
/*
winstar WEH001602A
*/

// commands
#define LCD_CLEARDISPLAY 		0x01
#define LCD_RETURNHOME 			0x02
#define LCD_ENTRYMODESET 		0x04
#define LCD_DISPLAYCONTROL 		0x08
#define LCD_CURSORSHIFT 		0x10
#define LCD_FUNCTIONSET 		0x20//28
#define LCD_SETCGRAMADDR 		0x40
#define LCD_SETDDRAMADDR 		0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 			0x00
#define LCD_ENTRYLEFT 			0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01 //0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00 //0x00
#define LCD_INCREMENTBIT 		0x02
#define LCD_DECREMENTBIT 		0x00

// flags for display on/off control
#define LCD_DISPLAYON 			0x04
#define LCD_DISPLAYOFF 			0x00
#define LCD_CURSORON 			0x02
#define LCD_CURSOROFF 			0x00
#define LCD_BLINKON 			0x01
#define LCD_BLINKOFF 			0x00

#define LCD_CURSORS_MASK 		0xFC

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 		0x08
#define LCD_CURSORMOVE 			0x00
#define LCD_MOVERIGHT 			0x04
#define LCD_MOVELEFT 			0x00

#define LCD_4BITMODE 			0x00
#define LCD_8BITMODE 			0x10
#define LCD_2LINE 				0x08
#define LCD_1LINE 				0x00
#define LCD_5x10DOTS 			0x04
#define LCD_5x8DOTS 			0x00
#define LCD_Right 				0
#define LCD_Left 				1
#define LCD_JAPANESE 			0x00
#define LCD_EUROPEAN_I 			0x01
#define LCD_EUROPEAN_II 		0x03
#define LCD_RUSSIAN 			0x02

#define LCD_SETGRAPHICMODE 		0x1F
#define LCD_SETCHARCTERMODE 	0x17

//GPIO
static const byte IOCON		 = 	0x0A;
static const byte IODIR 	 = 	0x00;
static const byte GPPU 		 =  0x0C;
static const byte GPIO 		 = 	0x12;
static const byte GPINTEN 	 = 	0x04;
static const byte IPOL 		 = 	0x02;
static const byte DEFVAL 	 = 	0x06;
static const byte INTF		 = 	0x0E;
static const byte INTCAP 	 = 	0x10;
static const byte OLAT 		 = 	0x14;
static const byte INTCON 	 = 	0x08;

// LCD PINS to GPIO PINS
static const uint8_t LCDDATAPINS[8]    = {0,1,2,3,4,5,6,7};
static const uint8_t LCDBUSYPIN		   =  LCDDATAPINS[7];// data

static const uint8_t LCDPIN_RW		   =	0;//8;// RW pin 		0B
static const uint8_t LCDPIN_RS		   =	1;//9;// rs pin         1B
static const uint8_t LCDPIN_EN 		   =	2;//10;// enable pin    2B
//3 unused
//4 unused
//5 unused
//6 unused
//7 unused

//multiple inheritance c++
class LiquidOLED : public Print {//
public:
	LiquidOLED(uint8_t address,uint8_t cspin);
    
	void 			begin(uint8_t cols, uint8_t rows,uint8_t charsize = LCD_5x8DOTS);
	void 			clear();
	void 			home();
	void 			setCursor(uint8_t col, uint8_t row);
	void 			noDisplay();
	void 			display();
	void 			noCursor();
	void 			cursor();
	void 			noBlink();
	void 			blink();
	void 			scrollDisplayLeft(void);
	void 			scrollDisplayRight(void);
	void 			leftToRight(void);
	void 			rightToLeft(void);
	void 			autoscroll(void);
	void 			noAutoscroll(void);
	void 			createChar(uint8_t location, uint8_t charmap[]);
	virtual size_t 	write(uint8_t);
	bool			getTextMode(void);
	bool			getGraphicMode(void);
	void			setTextMode(bool mode=1);
	void			setGraphicMode(bool mode=1);

private:
	int8_t			_x;					//current pos x
	int8_t			_y;					//current pos y
	byte 			_lcd_cols;			// phisical LCD width
	byte 			_lcd_lines;			// phisical LCD height
	uint8_t 		_displayfunction;
	uint8_t 		_displaycontrol;
	uint8_t 		_displaymode;
	uint8_t			_currentMode;
//------------------------------------------------------------
void 				send(uint8_t value, uint8_t mode);
inline void 		command(uint8_t value);
void 				pulseEnable(uint8_t value);
void 				write8bits(uint8_t value);
void 				write4bits(uint8_t value);
//------------------------------------------------------------
	uint8_t 		_address;
	uint8_t 		_cs_Pin;
	uint8_t 		_readCmd;
	uint8_t 		_writeCmd;
	byte			_lcdControlPins;//lo stato dei pin di controllo
	byte			_lcdDataPins;	//lo stato dei pin dati
	void 			gpioStartSend(bool mode);
	void 			gpioEndSend();
	void 			gpioWriteByte(byte reg, byte data);
	void 			gpioWriteWord(byte reg, uint16_t data);
	void 			gpioWriteBytes(byte dati, byte controllo);

};

#endif
