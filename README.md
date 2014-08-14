A driver for winstar <b>WEH001602A</b> Text OLED Module 16*2 that uses 3 wires.<br>

Attention! <br>
---> This driver uses a GPIO Chip called <b>MCP23S17</b>! (microchip) <---<br>
---> <u>Alpha development stage!</u> Text working 100%, still develop graphic mode <---<br>
<br>
Version:0.1a1<br>
<br>
<b>Preface:</b><br>
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
probably change but text mode it's 100% functional so it can be used.<br>
Another problem it's the winstar documentation...Terrible, incomplete, confused, whatever. Had to spend time to
digest it and not enough since I have to find a way by experiment in most cases!<br>
<b>Programming Note.</b><br>
I'm currently not using the 'wait for ready' routine at all! Reason it's the GPIO combined with slow winstar interface
has predictable delay that it's enough to avoid that. Things may change in future but can be added by software easily so currently it's enough fast for any applications!
<br>
<b>Note about Release Version (when ready)</b><br>
The release will work in text/graphic mode and with GPIO or in SPI (by change some jumper on the display.
The reason I still not planned the direct SPI mode is that I just have one display and waiting for a couple more
to experimenting modes.
<br>
<b>Connections:</b><br>
MCP23S17 --> WEH001602A display<br>
Can work at 3V3 or 5V.<br>

	  RW	-->	  IOB-0 [|   U  |] IOA-7	-->	D7<br>
	  RS  -->	  IOB-1 [| 	    |] IOA-6	-->	D6<br>
	  E   -->	  IOB-2 [| 	    |] IOA-5	-->	D5<br>
			      IOB-3 [| 	    |] IOA-4	-->	D4<br>
			      IOB-4 [| 	    |] IOA-3	-->	D3<br>
			      IOB-5 [| 	    |] IOA-2	-->	D2<br>
			      IOB-6 [| 	    |] IOA-1	-->	D1<br>
			      IOB-7 [| 	    |] IOA-0	-->	D0<br>
			      ++++  [| 	    |] [INT-A] not used<br>
			      GND   [| 	    |] [INT-B] not used<br>
			   	 CS     [| 	    |] RST (connect to +++)<br>
			     SCK    [| 	    |] A2	*see addressing<br>
			      MOSI  [| 	    |] A1   *see addressing<br>
		    [not used]  [|______|] A0	*see addressing<br>
		
	*Addressing (see MCP23s17 datasheet)<br>
	Ground all 3 will result in 0x20	<br>
---------------------------------------<br>
