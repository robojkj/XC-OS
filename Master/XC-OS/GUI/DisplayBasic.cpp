#include "DisplayPrivate.h"
#include "Basic/FileGroup.h"
#include "Basic/TasksManage.h"
#include "BSP/BSP.h"

TaskHandle_t TaskHandle_Display = 0;
TaskHandle_t TaskHandle_PageRun = 0;
QueueHandle_t SemHandle_FileSystem = 0;

uint8_t PORT_IO[16] = { TFT_D0_Pin, TFT_D1_Pin, TFT_D2_Pin, TFT_D3_Pin, TFT_D4_Pin, TFT_D5_Pin, TFT_D6_Pin, TFT_D7_Pin,
												TFT_D8_Pin, TFT_D9_Pin, TFT_D10_Pin, TFT_D11_Pin, TFT_D12_Pin, TFT_D13_Pin, TFT_D14_Pin, TFT_D15_Pin	};

SCREEN_CLASS screen(
    PORT_IO, TFT_RST_Pin, TFT_CS_Pin,
    TFT_RS_Pin, TFT_RW_Pin, TFT_RD_Pin
);

TP_CLASS tp_dev(Touch_INT_Pin);

touch_event touch = { 0 };

PageManager page(PAGE_MAX);

#define PAGE_REG(name)\
do{\
    extern void PageRegister_##name(uint8_t pageID);\
    PageRegister_##name(PAGE_##name);\
}while(0)

static void Init_Pages()
{
    PAGE_REG(Home);
    PAGE_REG(Settings);
    PAGE_REG(BattInfo);
    PAGE_REG(LuaScript);
    //PAGE_REG(LuaAppSel);
    //PAGE_REG(LuaAppWin);
    PAGE_REG(FileExplorer);
    PAGE_REG(TextEditor);
    PAGE_REG(WavPlayer);
    PAGE_REG(BvPlayer);
    PAGE_REG(Game);
    PAGE_REG(GameSelect);
    PAGE_REG(About);
    PAGE_REG(RadioCfg);
    PAGE_REG(Shell);
    PAGE_REG(SubDev);
    
    page.PagePush(PAGE_Home);
}

void Page_Delay(uint32_t ms)
{
    vTaskDelay(ms);
}


void Task_Display(void *pvParameters)
{
    DisplayError_Init();
    
    screen.begin();
	screen.setRotation(2);	//	ÆÁÄ»Ðý×ªÊÊÅä
    screen.fillScreen(screen.Black);
	
	tp_dev.begin();
    
    Backlight_SetValue(0);

    lv_init();
    lv_disp_init();
    lv_theme_set_current(lv_theme_material_init(200, NULL));
    lv_fsys_init();
    SemHandle_FileSystem = xSemaphoreCreateBinary();
    xSemaphoreGive(SemHandle_FileSystem);

    Init_Bar();
    Init_Pages();
    
    Backlight_SetGradual(500);

    for(;;)
    {
        lv_task_handler();
        vTaskDelay(10);
    }
}

void Task_PageRun(void *pvParameters)
{
    for(;;)
    {
        page.Running();
        vTaskDelay(20);
    }
}
