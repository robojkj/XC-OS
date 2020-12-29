#include "WavPlayer_Private.h"
#include "Lyric_Analyzer/Lyric_Analyzer.h"

#define LyricFloder "/lyrics"


static File LyricFile;
//static File LyricFile;
static bool HaveLyricFile = false;

/*从文件读取一行*/
static String Lyric_GetNextLine()
{
#if 0
    return LyricFile.readStringUntil('\n');
#endif
	return LyricFile.readStringUntil('\n');
}

/*实例化歌词解析器对象*/
static Lyric_Analyzer lyric(Lyric_GetNextLine, Wav_LabelLrcUpdate);

bool Lyric_Setup(String path)
{
#if 0
    HaveLyricFile = false;
    int index = path.lastIndexOf('/');
    if(index == -1)
        return false;
    
    String filename = path.substring(index);
    path = path.substring(0, index) + LyricFloder + filename;
    
    LyricFile = SD.open(path);
    
    if(!LyricFile)
        return false;

    LyricFile.setTimeout(5);
    
    if(path.endsWith(".xtrc"))
    {
        lyric.Start();
    }
    else if(path.endsWith(".xlrc") || path.endsWith(".lrc"))
    {
        lyric.Start(lyric.OutputMode_Full, lyric.DecodeMode_XLRC);
    }
    else
    {
        LyricFile.close();
        return false;
    }

    HaveLyricFile = true;
    return true;
#endif
	HaveLyricFile = false;
	int index = path.lastIndexOf('/');
	if (index == -1)
		return false;
    
	String filename = path.substring(index);
	path = path.substring(0, index) + LyricFloder + filename;
	
	if (!LyricFile.open(path.c_str(), FA_READ))
	{
		//Serial.println("LRC ER");
		return false;
	}
    
	if (path.endsWith(".xtrc"))
	{
		lyric.Start();
	}
	else if (path.endsWith(".xlrc") || path.endsWith(".lrc"))
	{
		lyric.Start(lyric.OutputMode_Full, lyric.DecodeMode_XLRC);
	}
	else
	{
		//Serial.println("LRC NULL");
		LyricFile.close();
		return false;
	}
	//Serial.println("LRC OK");
	HaveLyricFile = true;
	return true;
	
}

void Lyric_Loop(uint32_t tick)
{
    if(HaveLyricFile)
    {
        lyric.Running(tick);
    }
}

void Lyric_Exit()
{
#if 0
    LyricFile.close();
#endif
	//Serial.println("LRC CLOSE");
	LyricFile.close();
}




