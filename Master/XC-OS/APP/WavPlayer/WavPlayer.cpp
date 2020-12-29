#include "GUI/DisplayPrivate.h"
#include "Basic/TasksManage.h"
#include "FifoQueue/FifoQueue.h"
#include "WavPlayer_Private.h"

#include "MP3Decoder/mp3_decoder.h"

//#include "SdFat.h"

static uint32_t file_plan;

/*WAV缓冲队列*/
static uint8_t* waveBuff;
static FifoQueue<uint8_t> * WaveFifo;

/*WAV文件信息*/
//static File WavFile;
static File file;
static WAV_TypeDef Wav_Handle;

/*WAV播放控制*/
#define WAV_VOLUME_MAX 0.1f
static float Wav_Volume = WAV_VOLUME_MAX / 5;   //  设置开机音量
static bool Wav_Playing = true;
static float Wav_Volume_temp;   //音量缓冲做目标点

#define WAV_resolution 4    //位移6位是10位DAC，位移4位是12位DAC

/*WAV播放任务句柄*/
TaskHandle_t TaskHandle_WavPlayer;

#define WAVFIFO_RX_BUFFER_SIZE (6 * 1024)
#define WAVFIL_BUFFER_SIZE (WAVFIFO_RX_BUFFER_SIZE - 16)
static uint8_t *_file_buffer;
//dac缓冲区数量必须是文件读取的1/4倍
#define DAC_BUFFER_SIZE WAVFIL_BUFFER_SIZE / 4


typedef struct
{
    DMA_Stream_TypeDef *DMA_Stream;
    uint32_t DAC_PAR;       /* 外设地址 */
    uint16_t *buffer1;      /* 指向缓冲区指针 */
    uint16_t *buffer2;      /* 指向缓冲区指针 */
    uint8_t buf1_flag;      /* DMA传输完成标志 */
    uint8_t buf2_flag;      /* DMA传输完成标志 */
    uint8_t dma_index;      /* 数据缓存区选择标志 */
} DAC_Base_TypeDef;


static DAC_Base_TypeDef *DAC1_Base;
static DAC_Base_TypeDef *DAC2_Base;


enum { INDEX_BUF1 = 0, INDEX_BUF2 = 1 };
enum { FLAG_IDLE = 0, FLAG_BUSY = 1 };

static uint8_t decode_Mode;

static uint32_t _music_filter = 0;

short *mp3_buf_p;
int *mp3_buf_size;

static void WavTimer_Handler()
{
    #if 0
    uint32_t WavAvailable = WaveFifo->available();
    if(WavAvailable < 16)
    {
        return;
    }
    else
    {
    uint16_t data = ((Wav_Handle.CurrentData.LeftVal + 32768) >> 6) * Wav_Volume;
    uint16_t data2 = ((Wav_Handle.CurrentData.RightVal + 32768) >> 6) * Wav_Volume;
    Wav_Next_16Bit2Channel(&Wav_Handle);
    Audio_WriteData(data, data2);

    FFT_AddData(data);
    #endif

    if(DAC1_Base->buf1_flag == FLAG_BUSY)
    {
        FFT_AddData(DAC1_Base->buffer1[DMA_GetCurrDataCounter(DAC1_Base->DMA_Stream)] * 4);

    }
    else if(DAC1_Base->buf2_flag == FLAG_BUSY)
    {
        FFT_AddData(DAC1_Base->buffer2[DMA_GetCurrDataCounter(DAC1_Base->DMA_Stream)] * 4);
    }

    //}
}

static uint8_t WavFileLoader(HWAVEFILE handle, uint8_t size, uint8_t **buffer)
{
    static uint8_t buf_tmp[32];
    int bufferPos = 0;
    while(size --)
    {
        buf_tmp[bufferPos ++] = WaveFifo->read();
    }
    *buffer = buf_tmp;
    return 0;
}


static void WavFileRead()
{
    if(file_plan == 0)  return;

    uint32_t read_size;
    //限制最大读取buf数量
    if(file_plan > WAVFIL_BUFFER_SIZE)
        read_size = WAVFIL_BUFFER_SIZE - WaveFifo->available();
    //剩余字节限制
    if(read_size > file_plan)
        read_size = file_plan;

    file.read( _file_buffer, read_size);

    for(int i = 0; i < read_size; i++)
        WaveFifo->write(_file_buffer[i]);

    file_plan -= read_size;

#if 0
    while(WaveFifo->available() < WaveFifo->size() - 16)
    {

        uint8_t buffer[4];
        WavFile.read(buffer, sizeof(buffer));
        for(int i = 0; i < sizeof(buffer); i++)
        {
            WaveFifo->write(buffer[i]);
        }
    }
#endif
}


static void WavBufferUpdate()
{
    DAC_Base_TypeDef *DB1 = DAC1_Base;
    DAC_Base_TypeDef *DB2 = DAC2_Base;

    if(DB1->buf1_flag == FLAG_IDLE && DB2->buf1_flag == FLAG_IDLE)
    {
        DB1->buf1_flag = FLAG_BUSY;
        DB2->buf1_flag = FLAG_BUSY;
        _music_filter++;
        //u32 t1 = micros();
        WavFileRead();
        //Serial.println(micros() - t1);
        //u32 t1 = micros();
        for(int i = 0; i < DAC_BUFFER_SIZE; i++)
        {
            //  读四个_file_buffer等于一个_dac1_buffer1
            Wav_Next_16Bit2Channel(&Wav_Handle);
            uint16_t data = ((Wav_Handle.CurrentData.LeftVal + 32768) >> WAV_resolution) * Wav_Volume;  //右移6位是10位，右移4位是12位
            uint16_t data2 = ((Wav_Handle.CurrentData.RightVal + 32768) >> WAV_resolution) * Wav_Volume;

            DB1->buffer1[i] = data;
            DB2->buffer1[i] = data2;
        }
        //Serial.println(micros() - t1);
        //Serial.printf("fp1,%d\r\n", file_plan);
    }

    if(DB1->buf2_flag == FLAG_IDLE && DB2->buf2_flag == FLAG_IDLE)
    {
        DB1->buf2_flag = FLAG_BUSY;
        DB2->buf2_flag = FLAG_BUSY;

        WavFileRead();

        for(int i = 0; i < DAC_BUFFER_SIZE; i++)
        {
            //  读四个_file_buffer等于一个_dac1_buffer1
            Wav_Next_16Bit2Channel(&Wav_Handle);
            uint16_t data = ((Wav_Handle.CurrentData.LeftVal + 32768) >> WAV_resolution) * Wav_Volume;
            uint16_t data2 = ((Wav_Handle.CurrentData.RightVal + 32768) >> WAV_resolution) * Wav_Volume;

            DB1->buffer2[i] = data;
            DB2->buffer2[i] = data2;
        }
        //Serial.printf("fp2,%d\r\n", file_plan);
    }

    //音频文件前面的杂音过滤掉
    if(WavPlayer_GetPlaying() && _music_filter > 2)
    {
        Audio_SetEnable(true);
    }
}




static void MP3BufferUpdate()
{
    DAC_Base_TypeDef *DB1 = DAC1_Base;
    DAC_Base_TypeDef *DB2 = DAC2_Base;

    if(DB1->buf1_flag == FLAG_IDLE && DB2->buf1_flag == FLAG_IDLE)
    {
        //uint32_t m = millis();
        if(MP3FileRead())
        {
            //Serial.println(millis()-m);
            //Serial.println("N1");
            DB1->buf1_flag = FLAG_BUSY;
            DB2->buf1_flag = FLAG_BUSY;
            _music_filter++;

            //Serial.printf("OK, %X %d\r\n", mp3_buf_size, *mp3_buf_size);
            //PCM数据是16位的情况下，一个字节左声道，一个字节右声道，共2个字节
            for(int i = 0; i < *mp3_buf_size; i += 2)
            {
                int32_t LeftVal = mp3_buf_p[i];
                int32_t RightVal = mp3_buf_p[i + 1];

                DB1->buffer1[i / 2] = ((LeftVal + 32768) >> WAV_resolution) * Wav_Volume;
                DB2->buffer1[i / 2] = ((RightVal + 32768) >> WAV_resolution) * Wav_Volume;
            }
        }
        else
        {
            //防止声音卡顿
            _music_filter = 0;
            Audio_SetEnable(false);
            Serial.println("Prevent sound jams");
        }
    }

    if(DB1->buf2_flag == FLAG_IDLE && DB2->buf2_flag == FLAG_IDLE)
    {
        if(MP3FileRead())
        {
            //Serial.println("N2");
            DB1->buf2_flag = FLAG_BUSY;
            DB2->buf2_flag = FLAG_BUSY;

            //Serial.printf("OK, %X %d\r\n", mp3_buf_size, *mp3_buf_size);
            for(int i = 0; i < *mp3_buf_size; i += 2)
            {
                int32_t LeftVal = mp3_buf_p[i];
                int32_t RightVal = mp3_buf_p[i + 1];

                DB1->buffer2[i / 2] = ((LeftVal + 32768) >> WAV_resolution) * Wav_Volume;
                DB2->buffer2[i / 2] = ((RightVal + 32768) >> WAV_resolution) * Wav_Volume;
            }
        }
        else
        {
            //防止声音卡顿
            _music_filter = 0;
            Audio_SetEnable(false);
            //Serial.println("Prevent sound jams");
        }
    }

    //音频文件前面的杂音过滤掉
    if(WavPlayer_GetPlaying() && _music_filter > 2)
    {
        Audio_SetEnable(true);
    }
}


void dac_dma_doublebuffer(DAC_Base_TypeDef *dac_base)
{
    if(decode_Mode == DECODE_WAV)
    {
        if(dac_base->dma_index == INDEX_BUF1)
        {
            dac_base->buf1_flag = FLAG_IDLE;
            dac_base->dma_index = INDEX_BUF2;
            dma_Start(dac_base->DMA_Stream, dac_base->DAC_PAR, dac_base->buffer2, DAC_BUFFER_SIZE);
        }
        else
        {
            dac_base->buf2_flag = FLAG_IDLE;
            dac_base->dma_index = INDEX_BUF1;
            dma_Start(dac_base->DMA_Stream, dac_base->DAC_PAR, dac_base->buffer1, DAC_BUFFER_SIZE);
        }
    }
    else
    {
        //Serial.println("N3");
        if(dac_base->dma_index == INDEX_BUF1)
        {
            dac_base->buf1_flag = FLAG_IDLE;
            dac_base->dma_index = INDEX_BUF2;
            dma_Start(dac_base->DMA_Stream, dac_base->DAC_PAR, dac_base->buffer2, *mp3_buf_size / 2);
        }
        else
        {
            dac_base->buf2_flag = FLAG_IDLE;
            dac_base->dma_index = INDEX_BUF1;
            dma_Start(dac_base->DMA_Stream, dac_base->DAC_PAR, dac_base->buffer1, *mp3_buf_size / 2);
        }
    }
}

//DMA完成中断
void DAC1_EventHandler(void)
{
    dac_dma_doublebuffer(DAC1_Base);
}

void DAC2_EventHandler(void)
{
    dac_dma_doublebuffer(DAC2_Base);
}







//应用层开始

uint32_t __wav_datastart;

bool __music_start;
bool WavPlayer_LoadFile(String path)
{
#if 0
    WavFile = SD.open(path, O_RDONLY);
    if(!WavFile)
    {
        return false;
    }
#endif
    //Serial.println("WAV LOADING....");
    //等待上一个事件释放关闭
    while(__music_start)    vTaskDelay(2);
    /*
    if(__music_start)
    {
        file.close();
        __music_start = false;
        Serial.println("music bug...ok");
        return false;
    }
    */
    if (!file.open(path.c_str(), FA_READ))
    {
        //Serial.println("WAV LOAD ER");
        return false;
    }
    //Serial.println("WAV LOAD OK");
    __music_start = true;

    if(path.endsWith(".wav"))
    {
        decode_Mode = DECODE_WAV;
        path.replace(".wav", "");
    }
    else
    {
        decode_Mode = DECODE_MP3;
        path.replace(".mp3", "");
    }

    /*支持的歌词文件类型*/
    const String LryicSupport_Grp[] = {".xtrc", ".xlrc", ".lrc"};
    for(uint8_t i = 0; i < __Sizeof(LryicSupport_Grp); i++)
    {
        if(Lyric_Setup(path + LryicSupport_Grp[i]))
        {
            /*歌词文件匹配成功*/
            break;
        }
    }

    file_plan = file.getSize();

    if(decode_Mode == DECODE_WAV)
    {
        uint32_t buffSize = WAVFIFO_RX_BUFFER_SIZE;//MemPool_GetResidueSize();
        waveBuff = (uint8_t*)ta_alloc(buffSize);
        if(waveBuff == NULL)
        {
            Audio_SetEnable(false);
            Serial.println("waveBuff NULL");
            return false;
        }

        memset(waveBuff, 0, buffSize);
        WaveFifo = new FifoQueue<uint8_t>(waveBuff, buffSize);

        _file_buffer = (uint8_t*)ta_alloc(WAVFIL_BUFFER_SIZE);
        if(_file_buffer == NULL)
        {
            Audio_SetEnable(false);
            Serial.println("_file_buffer NULL");
            return false;
        }
        //此函数读取完缓冲区字节，所以后面FIFO有WAV开头的字节余剩
        while(WaveFifo->available() < WaveFifo->size() - 16)
        {
            WavFileRead();
        }
        Wav_StructInit(&Wav_Handle, WavFileLoader);
        Wav_Open(&Wav_Handle);

        __wav_datastart = file.tell();
    }
    else
    {
        _file_buffer = (uint8_t*)ta_alloc(MP3INPUTBUF_SIZE);
        if(_file_buffer == NULL)
        {
            Serial.printf("_file_buffer NULL\r\n");
            return false;   // 停止播放
        }

        //直接变量读的是申请的数据地址，&是指针地址
        //Serial.printf("%X,%X\r\n", _file_buffer, &_file_buffer);

        mp3_init(path.c_str(), file, &_file_buffer, &mp3_buf_p, &mp3_buf_size);

        //要清空标志 否则是BUG
        Wav_Handle.IsEnd = false;
    }
    //缓冲值初始化
    __ExecuteOnce(Wav_Volume_temp = Wav_Volume);

    //Serial.printf("fileSize:%.2fKB, offset:%0.2fKB\r\n", (float)file.getSize()/1024, (float)file.tell()/1024);

    //初始化DAC设备
    DAC1_Base = (DAC_Base_TypeDef*)ta_alloc(sizeof(DAC_Base_TypeDef));
    if(DAC1_Base == NULL)
    {
        Audio_SetEnable(false);
        Serial.println("DAC1_Base NULL");
        return false;
    }
    DAC2_Base = (DAC_Base_TypeDef*)ta_alloc(sizeof(DAC_Base_TypeDef));
    if(DAC2_Base == NULL)
    {
        Audio_SetEnable(false);
        Serial.println("DAC2_Base NULL");
        return false;
    }

    //Serial.printf("tell:%d, Size:%d\r\n", file.tell(), file.getSize());

    memset(DAC1_Base, 0, sizeof(DAC_Base_TypeDef));
    memset(DAC2_Base, 0, sizeof(DAC_Base_TypeDef));

    DAC1_Base->DMA_Stream = DMA1_Stream5;
    DAC1_Base->DAC_PAR = (uint32_t)&DAC->DHR12R1;
    DAC2_Base->DMA_Stream = DMA1_Stream6;
    DAC2_Base->DAC_PAR = (uint32_t)&DAC->DHR12R2;
    //选择播放格式
    uint16_t _audio_freq;
    uint16_t _buffer_size;
    if(decode_Mode == DECODE_WAV)
    {
        _buffer_size = DAC_BUFFER_SIZE;
        _audio_freq = Wav_Handle.Header.SampleFreq;
    }
    else
    {
        _audio_freq = 44100;    //  常规频率
        _buffer_size = MP3BUFFER_SIZE / 2;  //实际1个数组有2个通道的数据
    }

    DAC1_Base->buffer1 = (uint16*)ta_alloc(_buffer_size * 2);
    DAC1_Base->buffer2 = (uint16*)ta_alloc(_buffer_size * 2);
    DAC2_Base->buffer1 = (uint16*)ta_alloc(_buffer_size * 2);
    DAC2_Base->buffer2 = (uint16*)ta_alloc(_buffer_size * 2);

    //Serial.printf("%X,%X\r\n", DAC1_Base->buffer1, &DAC1_Base->buffer1);
    //Serial.printf("%X,%X\r\n", DAC1_Base->buffer2, &DAC1_Base->buffer2);
    //Serial.printf("%X,%X\r\n", DAC2_Base->buffer1, &DAC2_Base->buffer1);
    //Serial.printf("%X,%X\r\n", DAC2_Base->buffer2, &DAC2_Base->buffer2);

    if(DAC1_Base->buffer1 == NULL)
    {
        Audio_SetEnable(false);
        Serial.println("DAC1_Base->buffer1 NULL");
        return false;
    }

    if(DAC1_Base->buffer2 == NULL)
    {
        Audio_SetEnable(false);
        Serial.println("DAC1_Base->buffer2 NULL");
        return false;
    }

    if(DAC2_Base->buffer1 == NULL)
    {
        Audio_SetEnable(false);
        Serial.println("DAC2_Base->buffer1 NULL");
        return false;
    }

    if(DAC2_Base->buffer2 == NULL)
    {
        Audio_SetEnable(false);
        Serial.println("DAC2_Base->buffer2 NULL");
        return false;
    }

    /*
        Serial.printf("_file_bufferADDRESS: %X\r\n", _file_buffer);
        Serial.printf("DAC1_BaseADDRESS: %X\r\n", DAC1_Base);
        Serial.printf("DAC2_BaseADDRESS: %X\r\n", DAC2_Base);
        Serial.printf("DAC1_Base->buffer1ADDRESS: %X\r\n", *(&DAC1_Base->buffer1));
        Serial.printf("DAC2_Base->buffer2ADDRESS: %X\r\n", *(&DAC1_Base->buffer2));
        Serial.printf("DAC2_Base->buffer1ADDRESS: %X\r\n", *(&DAC2_Base->buffer1));
        Serial.printf("DAC2_Base->buffer2ADDRESS: %X\r\n", *(&DAC2_Base->buffer2));
    */

    //Serial.printf("freertos ram:%d  mempool ram:%d\r\n", xPortGetFreeHeapSize(), MemPool_GetResidueSize());

    //采样率假如最高40000hz 缓冲区10k换算2.560kb 即翻转64ms翻转一次 此期间必须有缓冲区数据代入 否则音质卡顿
    //缓冲区5k，换算后1.28kb 32ms
    //缓冲区2k，换算后512b ST官方例程采用512字节双缓冲    要求12ms完成
    dac_tim_dma_init(1, XC_TIM_WAVPLAYER, _audio_freq, &DAC1_Base->buffer1[0], NULL, _buffer_size, DAC1_EventHandler);
    dac_tim_dma_init(2, XC_TIM_WAVPLAYER, _audio_freq, &DAC2_Base->buffer1[0], NULL, _buffer_size, DAC2_EventHandler);

    Timer_SetInterrupt(XC_TIM_FFT, 30, WavTimer_Handler);
    _music_filter = 0;
//    clock_out = Timer_GetClockOut(TIM_WAVPLAYER);

    return true;
}


void Music_setProgress(uint32_t value)  //pos=0~1000
{
    if(decode_Mode == DECODE_WAV)
    {
        uint32_t pcmlength = file.getSize() - __wav_datastart;
        float pcmvalue = __Map(value, 0, 1000, 0.0f, pcmlength);
        uint32_t file_pos = pcmvalue;

        //取4的倍数
        file_pos /= 4;
        file_pos *= 4;
        Wav_Handle.DataPosition = file_pos;
        file.seekSet(file_pos);
    }
    else
    {
        float mp3pos = __Map(value, 0, 1000, 0.0f, file.getSize() / 1024);
        uint32_t file_pos = mp3pos;
        file_pos *= 1024;

        mp3_set_file_pos(file_pos);

        //Serial.printf("%d, %d\r\n", file_pos, file.getSize());
    }
}

uint16_t Music_get_Progress()   //pos=0~1000
{
    uint32_t pos;

    if(decode_Mode == DECODE_WAV)
    {
        pos = ((float)Wav_Handle.DataPosition / Wav_Handle.DataSize) * 1000;
    }
    else
    {
        float mp3pos = __Map(file.tell(), 0.0f, file.getSize(), 0, 1000);
        pos = mp3pos;
    }

    return pos;
}

uint32_t Music_get_cursec()
{
    uint32_t sec;
    if(decode_Mode == DECODE_WAV)
    {
        sec = Wav_Handle.DataPosition / Wav_Handle.Header.BytePerSecond;
    }
    else
    {
        sec = file.tell() * mp3_get_totsec() / file.getSize();
        if(sec == mp3_get_totsec()) Wav_Handle.IsEnd = true;

        //return (__totsec * __frames_count) / __totframes;
    }

    return sec;
}

uint32_t Music_get_totsec()
{
    int sec;

    if(Music_getdecode_Mode() == DECODE_WAV)
    {
        sec = Wav_Handle.DataSize / Wav_Handle.Header.BytePerSecond;
    }
    else
    {
        sec = mp3_get_totsec();
    }

    return sec;
}


uint8_t Music_getdecode_Mode()
{
    return decode_Mode;
}


void WavPlayer_SetEnable(bool en)
{
    TIM_Cmd(XC_TIM_WAVPLAYER, (FunctionalState)en);
    TIM_Cmd(XC_TIM_FFT, (FunctionalState)en);

    Wav_Playing = en;
    if(en)
    {
        /*获取到文件系统使用权*/
        if(xSemaphoreTake(SemHandle_FileSystem, 100) == pdTRUE)
        {
            xTaskNotifyGive(TaskHandle_WavPlayer);
        }
    }
    else
    {
        Audio_SetEnable(en);

        /*队列缓存清空*/
        if(WaveFifo)WaveFifo->flush();
        /*结束标志位置位*/
        Wav_Handle.IsEnd = true;
    }
}

static void WavPlayer_AnimCallback(void * obj, int16_t volume)
{
    if(volume > 100)
        volume = 100;

    Wav_Volume = __Map(volume, 0, 100, 0.0f, WAV_VOLUME_MAX);
}

static void WavPlayer_AnimEvent(lv_anim_t a)
{
    Wav_Volume_temp = Wav_Volume;
}

void WavPlayer_SetVolume(uint8_t volume)
{
    static lv_anim_t a;
    lv_obj_add_anim(
        NULL, &a,
        (lv_anim_exec_xcb_t)WavPlayer_AnimCallback,
        WavPlayer_GetVolume(), volume,
        1000,
        (lv_anim_ready_cb_t)WavPlayer_AnimEvent
    );
}


void WavPlayer_VolumeRush()
{
    static lv_anim_t a;
    lv_obj_add_anim(
        NULL, &a,
        (lv_anim_exec_xcb_t)WavPlayer_AnimCallback,
        __Map(Wav_Volume_temp, 0, WAV_VOLUME_MAX, 0, 100), 0,
        500,
        NULL,
        lv_anim_path_ease_in_out,
        1
    );
}


uint8_t WavPlayer_GetVolume()
{
    return __Map(Wav_Volume, 0, WAV_VOLUME_MAX, 0, 100);
}

void WavPlayer_SetPlaying(bool en)
{
    Wav_Playing = en;
    //停止/启动静音模式
    Audio_SetEnable(en);
    TIM_Cmd(XC_TIM_WAVPLAYER, (FunctionalState)en);
    TIM_Cmd(XC_TIM_FFT, (FunctionalState)en);
}

bool WavPlayer_GetPlaying()
{
    return Wav_Playing;
}

uint32_t WavPlayer_GetPlayTime()
{
    return Wav_Handle.DataPosition / Wav_Handle.Header.BytePerSecond;
}

const WAV_TypeDef* WavPlayer_GetWavHandle()
{
    return &Wav_Handle;
}

void Task_WavPlayer(void *pvParameters)
{
    /*检测SD卡是否插入*/
    pinMode(SD_CD_Pin, INPUT_PULLUP);
    if(digitalRead(SD_CD_Pin))
    {
        vTaskSuspend(TaskHandle_WavPlayer);
    }

    for(;;)
    {
        /*等待播放命令任务通知*/
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        /*缓冲区更新*/
        while(!Wav_Handle.IsEnd)
        {
            if(Wav_Playing)
            {
                uint32_t tick = Wav_Handle.DataPosition * 1000.0f / Wav_Handle.Header.BytePerSecond;
                Lyric_Loop(tick);
                if(decode_Mode == DECODE_WAV)
                {
                    WavBufferUpdate();
                }
                else
                {
                    MP3BufferUpdate();
                }
                __IntervalExecute(FFT_Process(), 30);
            }
            vTaskDelay(2);
        }

        /*播放器失能*/
        WavPlayer_SetEnable(false);
        /*文件关闭*/
#if 0
        WavFile.close();
#endif
        file.close();

        /*歌词解析器退出*/
        Lyric_Exit();

        //MemPool_ALLFree();

        if(Music_getdecode_Mode() == DECODE_MP3) mp3_deinit();

        ta_free(waveBuff);
        ta_free(_file_buffer);
        ta_free(DAC1_Base->buffer1);
        ta_free(DAC1_Base->buffer2);
        ta_free(DAC2_Base->buffer1);
        ta_free(DAC2_Base->buffer2);
        ta_free(DAC1_Base);
        ta_free(DAC2_Base);

        /*
        if(waveBuff != NULL) Serial.printf("waveBuffFREE: %X\r\n",  ta_free(waveBuff));
        Serial.printf("_file_bufferFREE: %X\r\n",  ta_free(_file_buffer));

        Serial.printf("DAC1_Base->buffer1FREE: %X\r\n", ta_free(DAC1_Base->buffer1));
        Serial.printf("DAC2_Base->buffer2FREE: %X\r\n", ta_free(DAC1_Base->buffer2));
        Serial.printf("DAC2_Base->buffer1FREE: %X\r\n", ta_free(DAC2_Base->buffer1));
        Serial.printf("DAC2_Base->buffer2FREE: %X\r\n", ta_free(DAC2_Base->buffer2));

        Serial.printf("DAC1_BaseFREE: %d\r\n", ta_free(DAC1_Base));
        Serial.printf("DAC2_BaseFREE: %d\r\n", ta_free(DAC2_Base));

        Serial.printf("DAC1_Base->buffer1ADDRESS: %X\r\n", *(&DAC1_Base->buffer1));
        Serial.printf("DAC2_Base->buffer2ADDRESS: %X\r\n", *(&DAC1_Base->buffer2));
        Serial.printf("DAC2_Base->buffer1ADDRESS: %X\r\n", *(&DAC2_Base->buffer1));
        Serial.printf("DAC2_Base->buffer2ADDRESS: %X\r\n", *(&DAC2_Base->buffer2));
        */

        /*
        Serial.printf("ta_num_free: %d\r\n", ta_num_free());
        Serial.printf("ta_num_used: %d\r\n", ta_num_used());
        Serial.printf("ta_num_fresh: %d\r\n", ta_num_fresh());
        Serial.printf("ta_check: %d\r\n", ta_check());

        Serial.println("MUSIC CLOSE");
        */
        /*归还文件系统使用权*/
        xSemaphoreGive(SemHandle_FileSystem);

        __music_start = false;
    }
}




