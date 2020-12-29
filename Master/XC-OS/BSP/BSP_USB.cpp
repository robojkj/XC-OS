#include "Basic/FileGroup.h"
#include "Basic/TasksManage.h"
#include "BSP.h"

#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h"
#include "SdioDriver/sdio_sdcard.h"

TaskHandle_t TaskHandle_USB_MSC = 0;

extern "C"
{
    extern USB_OTG_CORE_HANDLE  USB_OTG_dev;
    extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
    
#ifdef USE_USB_OTG_FS
    void OTG_FS_IRQHandler(void)
    {
        USBD_OTG_ISR_Handler (&USB_OTG_dev);
    }
#endif
}

__ALIGN_BEGIN USB_OTG_CORE_HANDLE     USB_OTG_dev __ALIGN_END ;


void Task_USB_MSC(void *pvParameters)
{
	__ExecuteOnce((
        USBD_Init(&USB_OTG_dev,
              USB_OTG_FS_CORE_ID,
              &USR_desc,
              &USBD_MSC_cb,
              &USR_cb)
    ));
    
    for(;;)
    {
        vTaskDelay(5);
    }
}







