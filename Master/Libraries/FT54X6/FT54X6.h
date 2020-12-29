
/************************************************************************
This library is for FT54X6 7 inch Touchscreens. Tested with ASUS ZENFONE MAX ZC550KL that has FT5446. 
This Library was compiled with Arduino IDE with the "blue pill" board (STM32F103C8)
The interrupt pin level for 3.3V is not recognised by  Arduino uno or Nano board.
It can be either connected Through a transistor or an ATMEGA IC on 3.3v level.
####DEVELOPED BY AYAN DEY (30/07/2020)
*************************************************************************/

#ifndef _FT54X6_H_
#define _FT54X6_H_


#define SCREEN_MAX_X 320
#define SCREEN_MAX_Y 480


/* FT54X6 definitions */
#define FT54X6_ADDRESS 0x38

#define FT54X6_MODE 0x00
#define FT54X6_MODE_WORKING 0x00
#define FT54X6_MODE_TESTING 0x04

#define FT54X6_GEST_ID 0x01
#define FT54X6_GEST_ID_MOVE_UP     0x10
#define FT54X6_GEST_ID_MOVE_LEFT   0x14
#define FT54X6_GEST_ID_MOVE_DOWN   0x18
#define FT54X6_GEST_ID_MOVE_RIGHT  0x1c
#define FT54X6_GEST_ID_ZOOM_IN     0x48
#define FT54X6_GEST_ID_ZOOM_OUT    0x49
#define FT54X6_GEST_ID_NO_GESTURE  0x00
//Gesture Not working register returns 0x00 always
#define FT54X6_STATUS 0x02

#define FT54X6_TOUCH1_XH 0x03
#define FT54X6_TOUCH1_XL 0x04
#define FT54X6_TOUCH1_YH 0x05
#define FT54X6_TOUCH1_YL 0x06

#define FT54X6_TOUCH2_XH 0x09
#define FT54X6_TOUCH2_XL 0x0a
#define FT54X6_TOUCH2_YH 0x0b
#define FT54X6_TOUCH2_YL 0x0c

#define FT54X6_TOUCH3_XH 0x0f
#define FT54X6_TOUCH3_XL 0x10
#define FT54X6_TOUCH3_YH 0x11
#define FT54X6_TOUCH3_YL 0x12

#define FT54X6_TOUCH4_XH 0x15
#define FT54X6_TOUCH4_XL 0x16
#define FT54X6_TOUCH4_YH 0x17
#define FT54X6_TOUCH4_YL 0x18

#define FT54X6_TOUCH5_XH 0x1b
#define FT54X6_TOUCH5_XL 0x1c
#define FT54X6_TOUCH5_YH 0x1d
#define FT54X6_TOUCH5_YL 0x1e




#include <Wire.h>
#include <Arduino.h>

struct TouchPoint {
	uint8_t M;//Mode
    uint8_t P;//Pressure
    uint16_t X;//Position x
    uint16_t Y;//Position Y
};

class FT54X6{
    private:
		uint8_t int_pin;
		uint8_t rotate;
    
		uint8_t readRegister8(uint8_t reg);
		void writeRegister8(uint8_t reg, uint8_t val) ;
		void setRotate_90(uint16_t *x, uint16_t *y);
		void setRotate_180(uint16_t *x, uint16_t *y);
		void setRotate_270(uint16_t *x, uint16_t *y);
		void swap_xy(uint16_t *x, uint16_t *y);

	public:

		FT54X6(uint8_t int_pin);
		byte begin();
		bool touched();
		uint8_t getNoOfTouches();
		TouchPoint getTouchLoc(uint8_t num);
		void setRotation(uint8_t r);
};

#endif


