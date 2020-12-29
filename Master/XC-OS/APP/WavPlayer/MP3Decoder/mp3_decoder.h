#ifndef __MP3DECODER_H__
#define __MP3DECODER_H__

#include "stdint.h"
#include "Arduino.h"
#include "Fatfs.h"


#define MP3_TITSIZE_MAX		40		//歌曲名字最大长度
#define MP3_ARTSIZE_MAX		40		//歌曲名字最大长度
#define MP3_FILE_BUF_SZ     5*1024	//MP3解码时,文件buf大小

//V1.0 说明
//1,支持16位单声道/立体声MP3的解码
//2,支持CBR/VBR格式MP3解码
//3,支持ID3V1和ID3V2标签解析
//4,支持所有比特率(MP3最高是320Kbps)解码
#pragma pack(1) //按1字节对齐
//ID3V1 标签 
typedef struct 
{
    u8 id[3];		   	//ID,TAG三个字母
    u8 title[30];		//歌曲名字
    u8 artist[30];		//艺术家名字
	u8 year[4];			//年代
	u8 comment[30];		//备注
	u8 genre;			//流派 
}ID3V1_Tag;

//ID3V2 标签头 
typedef struct 
{
    u8 id[3];		   	//ID
    u8 mversion;		//主版本号
    u8 sversion;		//子版本号
    u8 flags;			//标签头标志
    u8 size[4];			//标签信息大小(不包含标签头10字节).所以,标签大小=size+10.
}ID3V2_TagHead;

//ID3V2.3 版本帧头
typedef struct 
{
    u8 id[4];		   	//帧ID
    u8 size[4];			//帧大小
    u16 flags;			//帧标志
}ID3V23_FrameHead;

//MP3 Xing帧信息(没有全部列出来,仅列出有用的部分)
typedef struct 
{
    u8 id[4];		   	//帧ID,为Xing/Info
    u8 flags[4];		//存放标志
    u8 frames[4];		//总帧数
	u8 fsize[4];		//文件总大小(不包含ID3)
}MP3_FrameXing;
 
//MP3 VBRI帧信息(没有全部列出来,仅列出有用的部分)
typedef struct 
{
    u8 id[4];		   	//帧ID,为Xing/Info
	u8 version[2];		//版本号
	u8 delay[2];		//延迟
	u8 quality[2];		//音频质量,0~100,越大质量越好
	u8 fsize[4];		//文件总大小
	u8 frames[4];		//文件总帧数 
}MP3_FrameVBRI;


//MP3控制结构体
typedef struct 
{
    u8 title[MP3_TITSIZE_MAX];	//歌曲名字
    u8 artist[MP3_ARTSIZE_MAX];	//艺术家名字
    u32 totsec ;				//整首歌时长,单位:秒
    u32 cursec ;				//当前播放时长
	
    u32 bitrate;	   			//比特率
	u32 samplerate;				//采样率
	u16 outsamples;				//PCM输出数据量大小(以16位为单位),单声道MP3,则等于实际输出*2(方便DAC输出)
	
	u32 datastart;				//数据帧开始的位置(在文件里面的偏移)
}__mp3ctrl;

#pragma pack()    //取消自定义字节对齐方式


bool mp3_get_info(const char *pname, __mp3ctrl* pctrl);
bool MP3FileRead();


bool mp3_init(String path, File fp, uint8_t **fbp, short **buf, int **size);
void mp3_deinit();
uint32_t mp3_get_totsec();
void mp3_set_file_pos(uint32_t pos);

#define MP3INPUTBUF_SIZE	3000
#define MP3BUFFER_SIZE		2304









#endif


