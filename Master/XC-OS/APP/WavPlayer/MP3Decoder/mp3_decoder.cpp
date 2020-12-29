
#include "mp3_decoder.h"
#include "mp3dec.h"
#include "BSP/BSP_MemoryPool.h"
#include "Basic/SysConfig.h"


//#define MP3_DEBUG_PRINTF(format, ...) Serial.printf(format, ##__VA_ARGS__)
#define MP3_DEBUG_PRINTF(format, ...)


extern "C" {
extern void Convert_Mono(short *buffer);	   //函数声明
extern void Convert_Stereo(short *buffer);
}

static uint8_t *_file_buffer_p;
static File file_p;		//传递文件指针
__mp3ctrl  *mp3ctrl;	//mp3控制结构体 


//解析ID3V1 
//buf:输入数据缓存区(大小固定是128字节)
//pctrl:MP3控制器
//返回值:0,获取正常
//    其他,获取失败
u8 mp3_id3v1_decode(u8* buf,__mp3ctrl *pctrl)
{
	ID3V1_Tag *id3v1tag;
	id3v1tag = (ID3V1_Tag*)buf;
	if (strncmp("TAG", (char*)id3v1tag->id, 3) == 0)//是MP3 ID3V1 TAG
	{
		if(id3v1tag->title[0])	strncpy((char*)pctrl->title, (char*)id3v1tag->title, 30);
		if(id3v1tag->artist[0])	strncpy((char*)pctrl->artist, (char*)id3v1tag->artist, 30); 
	    //此处还可以添加其他信息
	}else return 1;
	return 0;
}
//解析ID3V2 
//buf:输入数据缓存区
//size:数据大小
//pctrl:MP3控制器
//返回值:0,获取正常
//    其他,获取失败
u8 mp3_id3v2_decode(u8* buf,u32 size,__mp3ctrl *pctrl)
{
	ID3V2_TagHead *taghead;
	ID3V23_FrameHead *framehead; 
	u32 t;
	u32 tagsize;	//tag大小
	u32 frame_size;	//帧大小 
	taghead = (ID3V2_TagHead*)buf; 
	if(strncmp("ID3", (const char*)taghead->id, 3) == 0)//存在ID3?
	{
		tagsize = ((u32)taghead->size[0] << 21) | ((u32)taghead->size[1] << 14)
				| ((u16)taghead->size[2] << 7) | taghead->size[3];//得到tag 大小
		pctrl->datastart = tagsize;		//得到mp3数据开始的偏移量
		if(tagsize>size) tagsize = size;	//tagsize大于输入bufsize的时候,只处理输入size大小的数据
		if(taghead->mversion < 3)
		{
			MP3_DEBUG_PRINTF("not supported mversion!\r\n");
			return 1;
		}
		t = 10;
		while(t < tagsize)
		{
			framehead = (ID3V23_FrameHead*)(buf+t);
			frame_size = ((u32)framehead->size[0] << 24) | ((u32)framehead->size[1] << 16)
						|((u32)framehead->size[2] << 8) | framehead->size[3];//得到帧大小
 			if (strncmp("TT2", (char*)framehead->id, 3) == 0 || strncmp("TIT2", (char*)framehead->id, 4) == 0)//找到歌曲标题帧,不支持unicode格式!!
			{
				strncpy((char*)pctrl->title, (char*)(buf + t + sizeof(ID3V23_FrameHead) + 1), min(frame_size - 1, MP3_TITSIZE_MAX - 1));
			}
 			if (strncmp("TP1", (char*)framehead->id, 3) == 0 || strncmp("TPE1", (char*)framehead->id, 4) == 0)//找到歌曲艺术家帧
			{
				strncpy((char*)pctrl->artist, (char*)(buf + t + sizeof(ID3V23_FrameHead) + 1), min(frame_size - 1, MP3_ARTSIZE_MAX - 1));
			}
			t += frame_size + sizeof(ID3V23_FrameHead);
		} 
	}else 
	{
		pctrl->datastart = 0;//不存在ID3,mp3数据是从0开始
	}
	return 0;
}






static HMP3Decoder	Mp3Decoder;		/* mp3解码指针	*/
static MP3FrameInfo	Mp3FrameInfo;	/* mp3帧信息  	*/

static uint8_t *read_ptr;
static int	read_offset = 0;				/* 读偏移指针 */
static int	bytes_left = 0;					/* 剩余字节数 */	
static uint32_t bw;            					/* File R/W count */
static uint32_t ucFreq;			/* 采样频率 */





uint32_t __totsec;
uint32_t __totframes;
uint32_t __frames_count;
//获取MP3基本信息
//pname:MP3文件路径
//pctrl:MP3控制信息结构体 
//返回值:0,成功
//    其他,失败
bool mp3_get_info(const char *pname, __mp3ctrl* pctrl)
{
	MP3_FrameXing* fxing;
	MP3_FrameVBRI* fvbri;
	uint8_t *buf;
	int offset = 0;
	u32 p;
	short samples_per_frame;	//一帧的采样个数
	u32 totframes;				//总帧数
	
	buf = (uint8_t*)MemPool_Malloc(5 * 1024);
	if(buf == NULL)
	{
		MP3_DEBUG_PRINTF("memory fail\r\n");
		MemPool_Free(buf);
		return false;
	}
	
	bool res = file_p.read(buf, 5 * 1024);
	if(!res)
	{
		MP3_DEBUG_PRINTF("file read fail\r\n");
		MemPool_Free(buf);
		return false;
	}
	
	//读取文件成功,开始解析ID3V2/ID3V1以及获取MP3信息
	mp3_id3v2_decode(buf, 5 * 1024, pctrl);	//解析ID3V2数据	
	
	file_p.seekSet(file_p.getSize() - 128);
	
	file_p.read(buf, 128);//读取最后的128个字节
	
	mp3_id3v1_decode(buf, pctrl);	//解析ID3V1数据
	
	//偏移到数据开始的地方
	//读取5K字节mp3数据
	file_p.seekSet(pctrl->datastart);
	
	file_p.read(buf, 5 * 1024);
	offset = MP3FindSyncWord(buf, file_p.tell());	//查找帧同步信息
	if(offset >= 0 && MP3GetNextFrameInfo(Mp3Decoder, &Mp3FrameInfo, &buf[offset]) == 0)//找到帧同步信息了,且下一阵信息获取正常	
	{
		p = offset + 4 + 32;
		fvbri = (MP3_FrameVBRI*)(buf + p);
		if(strncmp("VBRI", (char*)fvbri->id, 4) == 0)//存在VBRI帧(VBR格式)
		{
			MP3_DEBUG_PRINTF("is VBRI\r\n");
			if (Mp3FrameInfo.version == MPEG1)
				samples_per_frame = 1152;//MPEG1,layer3每帧采样数等于1152
			else
				samples_per_frame = 576;//MPEG2/MPEG2.5,layer3每帧采样数等于576
			totframes = ((u32)fvbri->frames[0] << 24) | ((u32)fvbri->frames[1] << 16) 
						| ((u16)fvbri->frames[2] << 8) | fvbri->frames[3];//得到总帧数
			pctrl->totsec = totframes * samples_per_frame / Mp3FrameInfo.samprate;//得到文件总长度
		}else	//不是VBRI帧,尝试是不是Xing帧(VBR格式)
		{
			MP3_DEBUG_PRINTF("no is VBRI\r\n");
			if (Mp3FrameInfo.version == MPEG1)	//MPEG1
			{
				p = Mp3FrameInfo.nChans == 2?32:17;
				samples_per_frame = 1152;	//MPEG1,layer3每帧采样数等于1152
			}else
			{
				p = Mp3FrameInfo.nChans == 2?17:9;
				samples_per_frame = 576;		//MPEG2/MPEG2.5,layer3每帧采样数等于576
			}
			p += offset + 4;
			fxing = (MP3_FrameXing*)(buf + p);
			if(strncmp("Xing", (char*)fxing->id,4) == 0 || strncmp("Info", (char*)fxing->id, 4) == 0)//是Xng帧
			{
				if(fxing->flags[3] & 0X01)//存在总frame字段
				{	
					totframes = ((u32)fxing->frames[0] << 24) | ((u32)fxing->frames[1] << 16)
								| ((u16)fxing->frames[2] << 8) | fxing->frames[3];//得到总帧数
					pctrl->totsec = totframes * samples_per_frame / Mp3FrameInfo.samprate;//得到文件总长度
					MP3_DEBUG_PRINTF("XING frame\r\n");
				}else	//不存在总frames字段
				{
					pctrl->totsec = file_p.getSize() / (Mp3FrameInfo.bitrate / 8);
					MP3_DEBUG_PRINTF("XING no frame\r\n");
				}
			}else 		//CBR格式,直接计算总播放时间
			{
				pctrl->totsec = file_p.getSize() / (Mp3FrameInfo.bitrate / 8);
				MP3_DEBUG_PRINTF("XING CBR\r\n");
			}
		}
		pctrl->bitrate = Mp3FrameInfo.bitrate;			//得到当前帧的码率
		pctrl->samplerate = Mp3FrameInfo.samprate; 	//得到采样率. 
		if(Mp3FrameInfo.nChans == 2)
			pctrl->outsamples = Mp3FrameInfo.outputSamps; //输出PCM数据量大小 
		else
			pctrl->outsamples = Mp3FrameInfo.outputSamps * 2; //输出PCM数据量大小,对于单声道MP3,直接*2,补齐为双声道输出
	}else//未找到同步帧
	{
		MP3_DEBUG_PRINTF("Synchronization frame not found\r\n");
		MemPool_Free(buf);
		return false;
	}
	__totframes = totframes;
	__totsec = pctrl->totsec;
	MemPool_Free(buf);
	return true;
}



/*
//得到当前播放时间
//fx:文件指针
//mp3x:mp3播放控制器
void mp3_get_curtime(FIL*fx,__mp3ctrl *mp3x)
{
	u32 fpos=0;  	 
	if(fx->File_CurOffset>mp3x->datastart)fpos=fx->File_CurOffset-mp3x->datastart;	//得到当前文件播放到的地方 
	mp3x->cursec=fpos*mp3x->totsec/(fx->File_Size-mp3x->datastart);	//当前播放到第多少秒了?	
	Wav_Handle.DataPosition;
}Wav_Handle.DataSize
*/


static short *mp3_tempbuf;
int outputSamps;
int *_mp3_dma_size;






bool MP3FileRead()
{
	short 	*Outpcmbuf;			 //输出数据指针
	
	//MP3_DEBUG_PRINTF("%d, %d\r\n", file.tell(), bytes_left);	
	//寻找帧同步，返回第一个同步字的位置
	read_offset = MP3FindSyncWord(read_ptr, bytes_left);
	//没有找到同步字
	if(read_offset < 0)
	{
		bw = file_p.read(_file_buffer_p, MP3INPUTBUF_SIZE);
		if(file_p.get_error() != FR_OK)
		{
			MP3_DEBUG_PRINTF("读取失败 -> %d\r\n", file_p.get_error());
			return false;	//应是报错退出
		}
		read_ptr = _file_buffer_p;
		bytes_left = bw;
		//MP3_DEBUG_PRINTF("read_offset < 0");
		return false;	//	重新进入查找帧同步
	}
	
	read_ptr += read_offset;					//偏移至同步字的位置
	bytes_left -= read_offset;				//同步字之后的数据大小	
	if(bytes_left < 1024)							//补充数据
	{
		/* 注意这个地方因为采用的是DMA读取，所以一定要4字节对齐  */
		int i = (uint32_t)(bytes_left) & 3;					//判断多余的字节
		if(i) i = 4 - i;									//需要补充的字节
		memcpy(_file_buffer_p + i, read_ptr, bytes_left);		//从对齐位置开始复制
		read_ptr = _file_buffer_p + i;						//指向数据对齐位置
		//补充数据
		bw = file_p.read(_file_buffer_p + bytes_left + i, MP3INPUTBUF_SIZE - bytes_left - i);
		bytes_left += bw;									//有效数据流大小
		//MP3_DEBUG_PRINTF("bytes_left < 1024");
	}
	
	//开始解码 参数：mp3解码结构体、输入流指针、输入流大小、输出流指针、数据格式
	
	Outpcmbuf = mp3_tempbuf;
	int err = MP3Decode(Mp3Decoder, &read_ptr, &bytes_left, Outpcmbuf, 0);
	
	//错误处理
	if (err != ERR_MP3_NONE)									
	{
		switch (err)
		{
			case ERR_MP3_INDATA_UNDERFLOW:
				MP3_DEBUG_PRINTF("ERR_MP3_INDATA_UNDERFLOW\r\n");
				bw = file_p.read(_file_buffer_p, MP3INPUTBUF_SIZE);
				read_ptr = _file_buffer_p;
				bytes_left = bw;
				break;
			case ERR_MP3_MAINDATA_UNDERFLOW:
				/* do nothing - next call to decode will provide more mainData */
				MP3_DEBUG_PRINTF("ERR_MP3_MAINDATA_UNDERFLOW\r\n");
				break;		
			default:
				MP3_DEBUG_PRINTF("UNKNOWN ERROR:%d\r\n", err);	
			
				// 跳过此帧
				if (bytes_left > 0)
				{
					bytes_left --;
					read_ptr ++;
				}
			
				break;
		}
	}
	else		//解码无错误，准备把数据输出到PCM
	{
		MP3GetLastFrameInfo(Mp3Decoder, &Mp3FrameInfo);	//获取解码信息				
		/* 输出到DAC */
		outputSamps = Mp3FrameInfo.outputSamps;	//PCM数据个数
		if (outputSamps > 0)
		{
			if (Mp3FrameInfo.nChans == 1)	//单声道
			{
				//单声道数据需要复制一份到另一个声道
				/*for (int i = outputSamps - 1; i >= 0; i--)
				{
					Outpcmbuf[i * 2] = Outpcmbuf[i];
					Outpcmbuf[i * 2 + 1] = Outpcmbuf[i];
				}
				outputSamps *= 2;*/
				Convert_Mono((short*)Outpcmbuf);
				outputSamps *= 2;
			}else	//立体声
			{
				//Convert_Stereo((short*)Outpcmbuf);
			}
		}
		
		/* 根据解码信息设置采样率 */
		if (Mp3FrameInfo.samprate == 0)
		{
			MP3_DEBUG_PRINTF("samprate error\r\n");
			return false;
		}else if (Mp3FrameInfo.samprate != ucFreq)	//采样率 
		{
			ucFreq = Mp3FrameInfo.samprate;
			
			MP3_DEBUG_PRINTF(" \r\n Bitrate       %dKbps", Mp3FrameInfo.bitrate/1000);
			MP3_DEBUG_PRINTF(" \r\n Samprate      %dHz", ucFreq);
			MP3_DEBUG_PRINTF(" \r\n BitsPerSample %db", Mp3FrameInfo.bitsPerSample);
			MP3_DEBUG_PRINTF(" \r\n nChans        %d", Mp3FrameInfo.nChans);
			MP3_DEBUG_PRINTF(" \r\n Layer         %d", Mp3FrameInfo.layer);
			MP3_DEBUG_PRINTF(" \r\n Version       %d", Mp3FrameInfo.version);
			MP3_DEBUG_PRINTF(" \r\n OutputSamps   %d", Mp3FrameInfo.outputSamps);
			MP3_DEBUG_PRINTF("\r\n");
			//I2S_AudioFreq_Default = 2，正常的帧，每次都要改速率
			if(ucFreq >= I2S_AudioFreq_Default)
			{
				//根据采样率修改I2S速率
				Timer_SetInterruptFreqUpdate(XC_TIM_WAVPLAYER, ucFreq);
			}
		}
		
		__frames_count++;
		
		//(总时长*已播放窗口数)/总窗口数=已播放秒数
		/*MP3_DEBUG_PRINTF("%d / %d, %d / %d\r\n",
					__frames_count,
					__totframes,
					(__totsec * __frames_count) / __totframes,
					__totsec);*/
		
		*_mp3_dma_size = outputSamps;
		//MP3_DEBUG_PRINTF("OK, %d\r\n", *_mp3_dma_size);
		return true;
	}//else 解码正常
	
	return false;
}



bool mp3_init(String path, File fp, uint8_t **fbp, short **buf, int **size)
{
	file_p = fp;
	
	_file_buffer_p = *fbp;
	
	//初始化MP3解码器
	Mp3Decoder = MP3InitDecoder();
	if(Mp3Decoder == NULL)
	{
		MP3_DEBUG_PRINTF("Initializes the Helix decoding library equipment\r\n");
		return false;	// 停止播放
	}
	
	//读取MP3信息
	mp3ctrl = (__mp3ctrl*)MemPool_Malloc(sizeof(__mp3ctrl));
	memset(mp3ctrl,0,sizeof(__mp3ctrl));//数据清零 
	bool res = mp3_get_info(path.c_str(), mp3ctrl);
	if(!res)
	{
		return false;
	}
	/*
	MP3_DEBUG_PRINTF(mp3ctrl->totsec);
	MP3_DEBUG_PRINTF(mp3ctrl->bitrate);
	MP3_DEBUG_PRINTF(mp3ctrl->samplerate);
	MP3_DEBUG_PRINTF(mp3ctrl->outsamples);
	MP3_DEBUG_PRINTF(mp3ctrl->datastart);
	*/
	MemPool_Free(mp3ctrl);
	
	mp3_tempbuf = (short*)MemPool_Malloc(MP3BUFFER_SIZE*2);
	if(mp3_tempbuf == NULL)
	{
		MP3_DEBUG_PRINTF("mp3_tempbuf NULL\r\n");
		return false;	/* 停止播放 */
	}
	
	_mp3_dma_size = (int*)MemPool_Malloc(sizeof(int));
	if(_mp3_dma_size == NULL)
	{
		MP3_DEBUG_PRINTF("_mp3_dma_size NULL\r\n");
		return false;	/* 停止播放 */
	}
	
	//解码次数累加 每播放一个窗口就+1
	__frames_count = 0;
	
	file_p.seekSet(0);
	//初始化文件，去掉MP3头文件
	file_p.read( _file_buffer_p, MP3INPUTBUF_SIZE);
	if(file_p.get_error() != FR_OK)
	{
		MemPool_Free(_file_buffer_p);
		_file_buffer_p = NULL;
		MP3_DEBUG_PRINTF("FileRead%sfail -> %d\r\n", file_p.get_error());
		MP3FreeDecoder(Mp3Decoder);
		return false;
	}
	
	read_ptr = _file_buffer_p;
	bytes_left = bw;

	ucFreq = 44100;
	
	*buf = mp3_tempbuf;
	*size = _mp3_dma_size;
	
	//MP3_DEBUG_PRINTF("%X %X\r\n", buf, size);
	//MP3_DEBUG_PRINTF("%X %X\r\n", mp3_tempbuf, _mp3_dma_size);
	return true;
}




void mp3_deinit()
{
	MP3FreeDecoder(Mp3Decoder);
}


void mp3_set_file_pos(uint32_t pos)
{
	file_p.seekSet(pos);
	bw = file_p.read(_file_buffer_p, MP3INPUTBUF_SIZE);
	read_ptr = _file_buffer_p;
	bytes_left = bw;
}




uint32_t mp3_get_totsec()
{
	return __totsec;
}


















