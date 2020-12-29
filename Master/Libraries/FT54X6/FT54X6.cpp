#include "FT54X6.h" 

bool tch_ = false;

//#define SERIAL_DEBUG

FT54X6::FT54X6(uint8_t INT_PIN)
{
    int_pin = INT_PIN;
}
void touch_isr()
{
	tch_= true;
}
byte FT54X6::begin()
{
    Wire_TP.begin();
    Wire_TP.beginTransmission(FT54X6_ADDRESS);
    Wire_TP.write((byte)FT54X6_MODE);
    Wire_TP.write((byte)FT54X6_MODE_WORKING);
    byte error = Wire_TP.endTransmission();

    #ifdef SERIAL_DEBUG
	if (error == 0){
		Serial.println("CTP_INIT_OK");
	}else{
		Serial.println("CTP_Fail_ERROR");
	}
    #endif
	
    pinMode(int_pin, INPUT);
    attachInterrupt(digitalPinToInterrupt(int_pin), touch_isr, FALLING);
	
    return error;
}


bool FT54X6::touched()
{
	if (tch_){
		tch_= false;
		return true;
	}else{
		return false;
	}
}


uint8_t FT54X6::readRegister8(uint8_t reg) {
	uint8_t x;
	// use i2c
	Wire_TP.beginTransmission(FT54X6_ADDRESS);
	Wire_TP.write((byte)reg);
	Wire_TP.endTransmission();

	Wire_TP.requestFrom((byte)FT54X6_ADDRESS, (byte)1);
	while (Wire_TP.available() > 0){
		x = Wire_TP.read();
	}

#ifdef I2C_DEBUG
	Serial.print("$");
	Serial.print(reg, HEX);
	Serial.print(": 0x");
	Serial.println(x, HEX);
#endif

  return x;
}

void FT54X6::writeRegister8(uint8_t reg, uint8_t val) {
	// use i2c
	Wire_TP.beginTransmission(FT54X6_ADDRESS);
	Wire_TP.write((byte)reg);
	Wire_TP.write((byte)val);
	Wire_TP.endTransmission();
}

void FT54X6::swap_xy(uint16_t *x, uint16_t *y)
{
    int temp = 0;

    temp = *x;
    *x = *y;
    *y = temp;
}


void FT54X6::setRotate_90(uint16_t *x, uint16_t *y)
{
	*x = SCREEN_MAX_X - 1 - *x;

    *x = (*x * SCREEN_MAX_Y) / SCREEN_MAX_X;
    *y = (*y * SCREEN_MAX_X) / SCREEN_MAX_Y;

    swap_xy(x, y);
}

void FT54X6::setRotate_180(uint16_t *x, uint16_t *y)
{
	*y = SCREEN_MAX_Y - 1 - *y;
    *x = SCREEN_MAX_X - 1 - *x;
}

void FT54X6::setRotate_270(uint16_t *x, uint16_t *y)
{
	*y = SCREEN_MAX_Y - 1 - *y;

    *x = (*x * SCREEN_MAX_Y) / SCREEN_MAX_X;
    *y = (*y * SCREEN_MAX_X) / SCREEN_MAX_Y;

    swap_xy(x, y);
}


TouchPoint FT54X6::getTouchLoc(uint8_t num)
{
	byte TOUCH_P = FT54X6_TOUCH1_XH+(num-1)*(byte)6;
	//TOUCH_PXH=TOUCH_P //TOUCH_PXL=TOUCH_P+1 //TOUCH_PYH=TOUCH_P+2//TOUCH_PYL=TOUCH_P+3
	uint16_t Touch_P_x = 0;
	byte x_ = readRegister8(TOUCH_P) &B00001111;
	Touch_P_x = Touch_P_x|x_;
	Touch_P_x = (Touch_P_x << 8) | readRegister8(TOUCH_P + (byte)1);

	uint16_t Touch_P_Y = 0;
	byte Y_ = readRegister8(TOUCH_P+(byte)2) & B00001111;
	Touch_P_Y = Touch_P_Y|Y_;
	Touch_P_Y = (Touch_P_Y << 8)|readRegister8(TOUCH_P + (byte)3);

	uint8_t mode = readRegister8(TOUCH_P)>>6;
	uint8_t pressure =  readRegister8(TOUCH_P + (byte)4);
	TouchPoint tp;
	tp.M = mode;
	tp.P = pressure;
	tp.X = Touch_P_x;
	tp.Y = Touch_P_Y;

	switch(rotate)
	{
		case 0:
			
			break;
		case 1:
			setRotate_90(&tp.X, &tp.Y);
			break;
		case 2:
			setRotate_180(&tp.X, &tp.Y);
			break;
		case 3:
			setRotate_270(&tp.X, &tp.Y);
			break;
		default:
			break;
	}

	return tp;
}

uint8_t FT54X6::getNoOfTouches()
{
  return readRegister8(0x02);
}


void FT54X6::setRotation(uint8_t r)
{
	rotate = r % 4;
}








