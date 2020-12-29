#ifndef __TFT_ILI9488_H
#define __TFT_ILI9488_H

#include "Arduino.h"
#include "Adafruit_GFX_Library/Adafruit_GFX.h"
//#include "StrPrinter.h"

#define UTFT_WIDTH  320
#define UTFT_HEIGHT 480

#define USE_FAST_IO

class UTFT : public Adafruit_GFX
{
public:
    UTFT(
				uint8_t d0[16], uint8_t rst, uint8_t cs,
				uint8_t rs, uint8_t rw, uint8_t rd
    );

    // Pass 8-bit (each) R,G,B, get back 16-bit packed color
    inline uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
    }

    typedef enum
    {
        White = 0xFFFF,
        Black = 0x0000,
        Blue = 0x001F,
        Bred = 0XF81F,
        Gred = 0XFFE0,
        Gblue = 0X07FF,
        Red = 0xF800,
        Magenta = 0xF81F,
        Green = 0x07E0,
        Cyan = 0x7FFF,
        Yellow = 0xFFE0,
        Brown = 0XBC40, //棕色
        Brred = 0XFC07, //棕红色
        Gray = 0X8430, //灰色

        DarkBlue = 0X01CF, //深蓝色
        LightBlue = 0X7D7C, //浅蓝色
        GRAYBLUE = 0X5458, //灰蓝色

        LightGreen = 0X841F, //浅绿色
        LightGray = 0XEF5B, //浅灰色(PANNEL)
        Lgray = 0XC618, //浅灰色(PANNEL),窗体背景色

        LgrayBlue = 0XA651, //浅灰蓝色(中间层颜色)
        Lbblue = 0X2B12 //浅棕蓝色(选择条目的反色)
    } Color_Type;
    
    void begin();

#ifdef __STRPRINTER_H__    
    int printfX(const char *fmt, ...);
    int printfX(String str);
    void setOpacityX(uint8_t opacity);
#endif
    
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
    virtual void setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
    virtual void setCursor(int16_t x, int16_t y);
    virtual void setRotation(uint8_t r);  
    virtual void fillScreen(uint16_t color);
    virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    virtual void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h);
    
    virtual void drawFastRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h);
    virtual void drawFastBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h);
	void TFT_SET_ADDR_WINDOW(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
	void writeCommond(uint8_t cmd);
	void writeData(uint16_t data);
	
	void lcdOff();
	void lcdOn();
	void setContrast(uint8_t c);
	void setBrightness(byte br);

private:
	
    uint8_t RST, CS, RS, RW, RD;
    uint8_t colstart, rowstart, xstart, ystart;
	uint8_t PORT_IO[16];

#if defined(USE_FAST_IO)
    #if defined(__STM32__) 
        GPIO_TypeDef *rstport, *csport, *rsport, *rwport, *rdport;
        uint16_t  rstpinmask, cspinmask, rspinmask, rwpinmask, rdpinmask;
		GPIO_TypeDef *d0port, *d1port, *d2port, *d3port, *d4port, *d5port, *d6port, *d7port,
					 *d8port, *d9port, *d10port, *d11port, *d12port, *d13port, *d14port, *d15port;
        uint16_t  d0pinmask, d1pinmask, d2pinmask, d3pinmask, d4pinmask, d5pinmask, d6pinmask, d7pinmask,
		d8pinmask, d9pinmask, d10pinmask, d11pinmask, d12pinmask, d13pinmask, d14pinmask, d15pinmask;
    #endif
#endif
};

#endif
