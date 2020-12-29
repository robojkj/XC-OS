#ifndef __GAMEDISP_H
#define __GAMEDISP_H

#define DISP_SEND_DATA(data)	screen.writeCommond(data)
#define DISP_SET_WINDOW(x0,y0,x1,y1)	screen.TFT_SET_ADDR_WINDOW(x0,y0,x1,y1)

/*
#define DISP_SEND_DATA(data) \
do{\
    screen.writeCommond(data);\
}while(0)

#define DISP_SET_WINDOW(x0,y0,x1,y1) \
do{\
	screen.TFT_SET_ADDR_WINDOW(x0,y0,x1,y1);\
}while(0)
*/
#endif
