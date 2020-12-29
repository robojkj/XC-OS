#include "UTFT.h"

#define TFT_CMD_SET_X 0x2A
#define TFT_CMD_SET_Y 0x2B
#define TFT_CMD_WRITE_RAM 0x2C
#define TFT_CMD_READ_RAM 0x2D

UTFT::UTFT(uint8_t d0[16], 
		uint8_t rst, uint8_t cs, uint8_t rs, uint8_t rw, uint8_t rd)
    : Adafruit_GFX(UTFT_WIDTH, UTFT_HEIGHT)
{
	for(uint16_t pin = 0; pin < 16; pin++)	PORT_IO[pin] = d0[pin];

    RST = rst;
    CS = cs;
    RS = rs;
    RW = rw;
    RD = rd;
}

void UTFT::begin()
{
	pinMode(RST, OUTPUT);
    pinMode(CS, OUTPUT_AF);
    pinMode(RS, OUTPUT_AF);
    pinMode(RW, OUTPUT_AF);
    pinMode(RD, OUTPUT_AF);
	
	digitalWrite(RST, HIGH);
	digitalWrite(CS, HIGH);
	digitalWrite(RS, HIGH);
	digitalWrite(RW, HIGH);
	digitalWrite(RD, HIGH);
	
	for(uint16_t pin = 0; pin <  16; pin++)
	{
		uint8_t P = PORT_IO[pin];
		pinMode(P, OUTPUT_AF);
		GPIO_PinAFConfig(PIN_MAP[P].GPIOx, GPIO_GetPinNum(P),GPIO_AF_FMC); 
	}
	GPIO_PinAFConfig(PIN_MAP[CS].GPIOx, GPIO_GetPinNum(CS),GPIO_AF_FMC);
	GPIO_PinAFConfig(PIN_MAP[RS].GPIOx, GPIO_GetPinNum(RS),GPIO_AF_FMC);
	GPIO_PinAFConfig(PIN_MAP[RW].GPIOx, GPIO_GetPinNum(RW),GPIO_AF_FMC);
	GPIO_PinAFConfig(PIN_MAP[RD].GPIOx, GPIO_GetPinNum(RD),GPIO_AF_FMC);
	
	FSMC_Init();
	
	delay(10);
	
    digitalWrite(RST, LOW);
    delay(100);
    digitalWrite(RST, HIGH);
    delay(50);
	
    //************* ILI9488初始化**********//

	
	writeCommond(0xB0);    //  设置厂商访问保护
	writeData(0x04);

	writeCommond(0x36);    //  设置地址模式
	writeData(0x00);   

	writeCommond(0x3A);    //  设置像素格式
	writeData(0x55);   

	writeCommond(0xB4);    //  设置显示模式和帧内存写入模式
	writeData(0x00); 

	writeCommond(0xC0);    //  面板驱动设置
	writeData(0x03);//0013 设置rgb对应寄存器cba/选择源驱动器输出的移动方向
	writeData(0xDF);//480  
	writeData(0x40);
	writeData(0x12);
	writeData(0x00);
	writeData(0x01);
	writeData(0x00);
	writeData(0x43);

	writeCommond(0xC1);//frame frequency   显示定时设置为正常模式
	writeData(0x05);//BCn,DIVn[1:0
	writeData(0x2f);//RTNn[4:0] 
	writeData(0x08);// BPn[7:0]
	writeData(0x08);// FPn[7:0]
	writeData(0x00);
	

	writeCommond(0xC4);
	writeData(0x63);
	writeData(0x00);
	writeData(0x08);
	writeData(0x08);

	writeCommond(0xC8);//Gamma
	writeData(0x06);
	writeData(0x0c);
	writeData(0x16);
	writeData(0x24);//26
	writeData(0x30);//32 
	writeData(0x48);
	writeData(0x3d);
	writeData(0x28);
	writeData(0x20);
	writeData(0x14);
	writeData(0x0c);
	writeData(0x04);
	
	
	writeData(0x06);
	writeData(0x0c);
	writeData(0x16);
	writeData(0x24);
	writeData(0x30);
	writeData(0x48);
	writeData(0x3d);
	writeData(0x28);
	writeData(0x20);
	writeData(0x14);
	writeData(0x0c);
	writeData(0x04);


	writeCommond(0xC9);//Gamma
	writeData(0x06);
	writeData(0x0c);
	writeData(0x16);
	writeData(0x24);//26
	writeData(0x30);//32 
	writeData(0x48);
	writeData(0x3d);
	writeData(0x28);
	writeData(0x20);
	writeData(0x14);
	writeData(0x0c);
	writeData(0x04);

	writeData(0x06);
	writeData(0x0c);
	writeData(0x16);
	writeData(0x24);
	writeData(0x30);
	writeData(0x48);
	writeData(0x3d);
	writeData(0x28);
	writeData(0x20);
	writeData(0x14);
	writeData(0x0c);
	writeData(0x04);



	writeCommond(0xCA);//Gamma
	writeData(0x06);
	writeData(0x0c);
	writeData(0x16);
	writeData(0x24);//26
	writeData(0x30);//32 
	writeData(0x48);
	writeData(0x3d);
	writeData(0x28);
	writeData(0x20);
	writeData(0x14);
	writeData(0x0c);
	writeData(0x04);

	writeData(0x06);
	writeData(0x0c);
	writeData(0x16);
	writeData(0x24);
	writeData(0x30);
	writeData(0x48);
	writeData(0x3d);
	writeData(0x28);
	writeData(0x20);
	writeData(0x14);
	writeData(0x0c);
	writeData(0x04);


	writeCommond(0xD0);
	writeData(0x95);
	writeData(0x06);
	writeData(0x08);
	writeData(0x10);
	writeData(0x3f);


	writeCommond(0xD1);
	writeData(0x02);
	writeData(0x28);
	writeData(0x28);
	writeData(0x40);

	writeCommond(0xE1);    
	writeData(0x00);    
	writeData(0x00);    
	writeData(0x00);    
	writeData(0x00);    
	writeData(0x00);   
	writeData(0x00);   

	writeCommond(0xE2);    
	writeData(0x80);    

	writeCommond(0x2A);    //x,319
	writeData(0x00);    
	writeData(0x00);    
	writeData(0x01);    
	writeData(0x3F);    

	writeCommond(0x2B);    //y,479
	writeData(0x00);    
	writeData(0x00);    
	writeData(0x01);    
	writeData(0xDF);    

	writeCommond(0x11);

	delay(10);

	writeCommond(0x29);

	writeCommond(0xC1);//frame frequency
	writeData(0x05);//BCn,DIVn[1:0
	writeData(0x2f);//RTNn[4:0] 
	writeData(0x08);// BPn[7:0]
	writeData(0x08);// FPn[7:0]
	writeData(0x00);

	writeCommond(0x20);
	
}

void UTFT::TFT_SET_ADDR_WINDOW(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    FSMC_WriteCommond(TFT_CMD_SET_X);
    FSMC_WriteData((x0) >> 8);
    FSMC_WriteData(0x00FF & (x0));
    FSMC_WriteData((x1) >> 8);
    FSMC_WriteData(0x00FF & (x1));
    FSMC_WriteCommond(TFT_CMD_SET_Y);
    FSMC_WriteData((y0) >> 8);
    FSMC_WriteData(0x00FF & (y0));
    FSMC_WriteData((y1) >> 8);
    FSMC_WriteData(0x00FF & (y1));
    FSMC_WriteCommond(TFT_CMD_WRITE_RAM);
}

void UTFT::writeCommond(uint8_t cmd)
{
    FSMC_WriteCommond(cmd);
}

void UTFT::writeData(uint16_t data)
{
    FSMC_WriteData(data);
}


#ifdef __STRPRINTER_H__
extern "C" {
#include <stdio.h>
#include <stdarg.h>
}
void UTFT::setOpacityX(uint8_t opacity)
{
    SP_Brush_Opacity = opacity;
}
int UTFT::printfX(const char *fmt, ...)
{
    static char TEXT_BUFFER[128];
    va_list  va;
    va_start(va, fmt);
    int ret = vsprintf(TEXT_BUFFER, fmt, va);
    va_end(va);

    SP_PrepareRect(Fonts_MicrosoftYahei11px, (char *)TEXT_BUFFER, 0);
    SP_FillBuffer_SolidBrush(textbgcolor);
    SP_CoverString(Fonts_MicrosoftYahei11px, (char *)TEXT_BUFFER, 0, textcolor);

    drawRGBBitmap(cursor_x, cursor_y, SP_Buffer, SP_Buffer_Width, SP_Buffer_Height);
    cursor_x += SP_Buffer_Width;

    return ret;
}

int UTFT::printfX(String str)
{
    return printfX((const char*)str.c_str());
}
#endif

void UTFT::setRotation(uint8_t r)
{
    rotation = r % 4;
    switch(rotation)
    {
        case 0:
        _width = UTFT_WIDTH;
        _height = UTFT_HEIGHT;
        writeCommond(0x36);
        writeData(0 << 3);
        break;
    case 1:
        _width = UTFT_HEIGHT;
        _height = UTFT_WIDTH;
        writeCommond(0x36);
        writeData((0 << 3) | (1 << 6) | (1 << 5));
		//	bit3	RGB/BGR Order	‘0’ = RGB.	‘1’ = BGR.
		//	bit5	Page/Column Addressing Order	‘0’ = Normal Mode.	‘1’ = Reverse Mode.
		//	bit6	Column Address Order	‘0’ = Left to Right.	‘1’ = Right to Left.
		//	bit7	Page Address Order		‘0’ = Top to Bottom.	‘1’ = Bottom to Top.
        break;
    case 2:
        _width = UTFT_WIDTH;
        _height = UTFT_HEIGHT;
        writeCommond(0x36);
        writeData((0 << 3) | (1 << 6) | (1 << 7));
        break;
    case 3:
        _width = UTFT_HEIGHT;
        _height = UTFT_WIDTH;
        writeCommond(0x36);
        writeData((0 << 3) | (1 << 5) | (1 << 7));
        break;
    default:
        break;
    }
}

void UTFT::setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
    if(x0 < 0 || y0 < 0 || x1 < 0 || y1 < 0)
        return;
    
    writeCommond(TFT_CMD_SET_X);
    writeData(x0 >> 8);
    writeData(0x00FF & x0);
    writeData(x1 >> 8);
    writeData(0x00FF & x1);

    writeCommond(TFT_CMD_SET_Y);
    writeData(y0 >> 8);
    writeData(0x00FF & y0);
    writeData(y1 >> 8);
    writeData(0x00FF & y1);
    
    writeCommond(TFT_CMD_WRITE_RAM);
}

void UTFT::setCursor(int16_t x, int16_t y)
{
    cursor_x = x;
    cursor_y = y;
    if((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
    setAddrWindow(x, y, x + 1, y + 1);
}

void UTFT::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
    setAddrWindow(x, y, x + 1, y + 1);
    writeData(color);
}

void UTFT::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    // Rudimentary clipping
    if((x >= _width) || (y >= _height) || h < 1) return;
    if((y + h - 1) >= _height) h = _height - y;
    if(x < 0) x = 0;
    if(y < 0) y = 0;

    setAddrWindow(x, y, x, y + h - 1);

    while (h--)
        writeData(color);
}

void UTFT::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    // Rudimentary clipping
    if((x >= _width) || (y >= _height) || w < 1) return;
    if((x + w - 1) >= _width)  w = _width - x;
    if(x < 0) x = 0;
    if(y < 0) y = 0;

    setAddrWindow(x, y, x + w - 1, y);

    while (w--)
        writeData(color);
}

void UTFT::drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h)
{
    if((x >= _width) || (y >= _height)) return;

    int16_t actual_cursor_x = x;
    int16_t actual_cursor_y = y;
    int16_t actual_cursor_x1 = x + w - 1;
    int16_t actual_cursor_y1 = y + h - 1;

    if(actual_cursor_x < 0)
    {
        actual_cursor_x = 0;
    }
    else if(actual_cursor_x >= _width)
    {
        actual_cursor_x = _width - 1;
    }

    if(actual_cursor_y < 0)
    {
        actual_cursor_y = 0;
    }
    else if(actual_cursor_y >= _height)
    {
        actual_cursor_y = _height - 1;
    }

    if(actual_cursor_x1 < 0)
    {
        actual_cursor_x1 = 0;
    }
    else if(actual_cursor_x1 >= _width)
    {
        actual_cursor_x1 = _width - 1;
    }

    if(actual_cursor_y1 < 0)
    {
        actual_cursor_y1 = 0;
    }
    else if(actual_cursor_y1 >= _height)
    {
        actual_cursor_y1 = _height - 1;
    }
	
    TFT_SET_ADDR_WINDOW(actual_cursor_x, actual_cursor_y, actual_cursor_x1, actual_cursor_y1);
	
    for(int16_t Y = 0; Y < h; Y++)
    {
        for(int16_t X = 0; X < w; X++)
        {
            int16_t index = X + Y * w;
            int16_t actualX = x + X;
            int16_t actualY = y + Y;
            if(actualX >= 0 && actualX < _width && actualY >= 0 && actualY < _height)
            {
                writeData(bitmap[index]);
            }
        }
    }
}

void UTFT::drawFastRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h)
{
    TFT_SET_ADDR_WINDOW(x, y, (x + w - 1), (y + h - 1));
    uint32_t size = w * h;
    
    for(uint32_t i = 0; i < size; i++)
    {
        writeData(bitmap[i]);
    }
}

void UTFT::drawFastBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h)
{
    TFT_SET_ADDR_WINDOW(x, y, (x + w - 1), (y + h - 1));

    int16_t byteWidth = (w + 7) / 8;
    uint8_t b = 0;

    for(int16_t j = 0; j < h; j++)
    {
        for(int16_t i = 0; i < w; i++ )
        {
            if(i & 7) b <<= 1;
            else      b   = bitmap[j * byteWidth + i / 8];
            writeData((b & 0x80) ? White : Black);
        }
    }
}

void UTFT::fillScreen(uint16_t color)
{
    TFT_SET_ADDR_WINDOW(0, 0, (_width - 1), (_height - 1));
    uint32_t size = _width * _height;
    
    for(uint32_t i = 0; i < size; i++)
    {
        writeData(color);
    }
}

void UTFT::lcdOff()
{
	writeCommond(0x28);
}

void UTFT::lcdOn()
{
	writeCommond(0x29);
}

void UTFT::setContrast(uint8_t c)
{
	if (c > 64) c = 64;
	writeCommond(0x25);
	writeData(c);
}

void UTFT::setBrightness(byte br)
{
	if (br > 16) br = 16;
	writeCommond(0x01);
	writeData(br);
	writeCommond(0x0F);
}


