#include "Fatfs.h"

Fatfs::Fatfs(uint8_t id){
    _id[0] = id + '0';
	_id[1] = ':';
}
//为磁盘注册工作区	 
//path:磁盘路径，比如"0:"、"1:"
//mt:0，不立即注册（稍后注册）；1，立即注册
//返回值:执行结果
bool Fatfs::init()
{
    return f_mount(&fs, (const TCHAR*)_id, 1) == FR_OK ? true : false; 
}

bool Fatfs::close()
{
    return f_mount(NULL, (const TCHAR*)_id, 1) == FR_OK ? true : false; 
}

bool Fatfs::getSize(uint32_t* cardSize, uint32_t* volFree)
{
	FATFS *fs;
	DWORD fre_clust, fre_sect, tot_sect;
	
	//得到磁盘信息及空闲簇数量
	res = f_getfree(_id, &fre_clust, &fs);
	if(res == FR_OK)
	{
		/* 计算得到总的扇区个数和空扇区个数 */
		tot_sect = (fs->n_fatent - 2) * fs->csize;	//得到总扇区数
		fre_sect = fre_clust * fs->csize;			//得到空闲扇区数
		
		#if _MAX_SS!=512
		tot_sect*=fs->ssize/512;
		fre_sect*=fs->ssize/512;
		#endif
		
		/*获取储存器大小*/
		*cardSize = (uint32_t)(tot_sect);	//单位为KB
		/*获取储存器剩余空间的大小*/
		*volFree = (uint32_t)(fre_sect);	//单位为KB

	}
	
	return res == FR_OK ? true : false; 
}

//格式化
//path:磁盘路径，比如"0:"、"1:"
//mode:模式
//au:簇大小
//返回值:执行结果
bool Fatfs::mkfs()
{
	return f_mkfs((const TCHAR*)_id, 0, 0) == FR_OK ? true : false; //格式化,drv:盘符;mode:模式;au:簇大小
}

//获取盘符（磁盘名字）
//path:磁盘路径，比如"0:"、"1:"  
bool Fatfs::getlabel(char *path)
{
	char buf[20];
	uint32_t sn = 0;
	
	res = f_getlabel((const TCHAR *)path,(TCHAR *)buf,(DWORD*)&sn);
	if(res==FR_OK)
	{
		printf("\r\n磁盘%s 的盘符为:%s\r\n",path,buf);
		printf("磁盘%s 的序列号:%X\r\n\r\n",path,sn); 
	}else printf("\r\n获取失败，错误码:%X\r\n",res);
	return res == FR_OK ? true : false; 
}
//设置盘符（磁盘名字），最长11个字符！！，支持数字和大写字母组合以及汉字等
//path:磁盘号+名字，比如"0:ALIENTEK"、"1:OPENEDV"  
bool Fatfs::setlabel(char *path)
{
	res = f_setlabel((const TCHAR *)path);
	if(res==FR_OK)
	{
		printf("\r\n磁盘盘符设置成功:%s\r\n",path);
	}else printf("\r\n磁盘盘符设置失败，错误码:%X\r\n",res);
	return res == FR_OK ? true : false; 
}

FRESULT Fatfs::get_error()
{
	return res;
}












/***文件操作***/

#define q_malloc	pvPortMalloc	//malloc
#define q_free		vPortFree	//free

File::File(){
    #if _USE_LFN
	static char lfn[_MAX_LFN * 2 + 1];
	cur_fno.lfname = lfn;
	cur_fno.lfsize = sizeof(lfn);
	#endif
}


//打开路径下的文件
//path:路径+文件名
//mode:打开模式
//返回值:执行结果
bool File::open(const char *path, uint8_t mode)
{
	file = q_malloc(sizeof(FIL));
	res = f_open((FIL*)file, (const  TCHAR*)path, mode);//打开文件夹
	if(res != FR_OK)	q_free(file);
	else	state(path);
	
	return res == FR_OK ? true : false; 
}


bool File::close()
{
	res = f_close((FIL*)file);
	if(res == FR_OK && file != NULL)
	{
		q_free(file);
		file = NULL;
	}
	
	return res == FR_OK ? true : false; 
}

bool File::isOpen()
{
	return file == NULL ? false : true;
}

int File::write(uint8_t c)
{
    return f_putc((TCHAR)c, (FIL*)file);
}

int File::write(const TCHAR* c)
{
    return f_puts((const TCHAR*)c, (FIL*)file);
}

int File::write(const void* buff, uint32_t Size)
{
	FRESULT res;
	UINT i;
	
    res = f_write((FIL*)file, buff, Size, &i);

    if(res != FR_OK) return 0;

    return i;
}

extern "C" {
#include <stdio.h>
#include <stdarg.h>
}

// Work in progress to support printf.
// Need to implement stream FILE to write individual chars to chosen serial port

int File::printf (const char *__restrict __format, ...)
{
    char printf_buff[PRINTF_BUFFER_LENGTH];

    va_list args;
    va_start(args, __format);
    int ret_status = vsnprintf(printf_buff, sizeof(printf_buff), __format, args);
    //int ret_status = vsprintf(printf_buff,__format, args);
    va_end(args);
    write(printf_buff, PRINTF_BUFFER_LENGTH);

    return ret_status;
}

int File::read()
{
	uint8_t b;
	if(!read(&b, 1))	return 0;
    return b;
}

int File::read(void* buff, uint32_t Size)
{
	UINT i;
	
    res = f_read((FIL*)file, buff, Size, &i);

    if(res != FR_OK) return 0;

    return i;
}


String File::readStringUntil(char terminator)
{
    String ret;
	_startMillis = millis();
    int c = read();
    while (c >= 0 && c != terminator)
    {
        ret += (char)c;
		c = read();
		if (millis() - _startMillis > 10)  return "";
    }
    return ret;
}

FRESULT File::get_error()
{
	return res;
}

//文件读写指针偏移
//offset:相对首地址的偏移量
//返回值:执行结果.
bool File::seekSet(uint64_t offset)
{
	res = f_lseek((FIL*)file, offset);
    return res == FR_OK ? true : false; 
}

//读取文件当前读写指针的位置.
//返回值:位置
uint32_t File::tell()
{
    return f_tell((FIL*)file);
}

//读取文件大小
//返回值:文件大小
uint32_t File::getSize()
{
	return f_size((FIL*)file);
}

//读取文件名称
char* File::getName()
{
	char *fn;
	#if _USE_LFN
	fn=*cur_fno.lfname ? cur_fno.lfname : cur_fno.fname;
	#else
	fn=fileinfo.fname;;
	#endif
	return fn;
}

//获取文件日期
uint32_t File::getDate()
{
	return cur_fno.fdate;
}

//获取文件时间
uint32_t File::getTime()
{
	return cur_fno.ftime;
}

//获取文件属性
uint32_t File::getAttrib()
{
	return cur_fno.fattrib;
}


//文件状态
bool File::state(const TCHAR* path)
{
	res = f_stat((const TCHAR*)path, &cur_fno);
	return res == FR_OK ? true : false; 
}

//删除文件/目录
//pname:文件/目录路径+名字
//返回值:执行结果
bool File::unlink(const char *path)
{
	res = f_unlink((const TCHAR *)path);
	return res == FR_OK ? true : false; 
}

//修改文件/目录名字(如果目录不同,还可以移动文件哦!)
//oldname:之前的名字
//newname:新名字
//返回值:执行结果
bool File::rename(const char *oldname, const char* newname)
{
	res = f_rename((const TCHAR *)oldname,(const TCHAR *)newname);
	return res == FR_OK ? true : false; 
}

//文件或目录是否存在
bool File::exists(const char *path)
{
/*
while(*path != '\0')
{
	Serial.print(*path++);
	i++;
}
*/
	if(!state(path))
	{
		//或许是不带符号的路径
		if(get_error() == 6)
		{
			bool ret = opendir((const char *)path);
			if(ret) closedir();
			return ret;
		}
		return false;
	}
	
	if(isFile())
	{
		bool ret = open((const char *)path, FA_READ);
		if(ret) close();
		return ret;
	}
	else
	{
		bool ret = opendir((const char *)path);
		if(ret) closedir();
		return ret;
	}
}

//创建目录
//pname:目录路径+名字
//返回值:执行结果
bool File::mkdir(const char* path)
{
	res = f_mkdir((const TCHAR *)path);
	return res == FR_OK ? true : false;
}


//打开目录
 //path:路径
//返回值:执行结果
bool File::opendir(const char *path)
{
	dir_obj = q_malloc(sizeof(DIR));
	res = f_opendir((DIR*)dir_obj,(const TCHAR*)path);//打开文件夹
	if(res != FR_OK)	q_free(dir_obj);
	else
	{
		//#if _USE_LFN
		//cur_fno.lfsize = _MAX_LFN * 2 + 1;
		//cur_fno.lfname = (char*)q_malloc(sizeof(cur_fno.lfsize));
		//#endif
	}
	
	return res == FR_OK ? true : false;
}

//关闭目录 
//返回值:执行结果
bool File::closedir()
{
	res = f_closedir((DIR*)dir_obj);
	if(res == FR_OK && dir_obj != NULL)
	{
		q_free(dir_obj);
		dir_obj = NULL;
		//q_free(cur_fno.lfname);
	}
	
	return res == FR_OK ? true : false;
}

//读取一个目录项
bool File::readdir()
{
	res = f_readdir((DIR*)dir_obj,&cur_fno);//读取一个文件的信息
	if(res != FR_OK || cur_fno.fname[0] == 0)
	{
		return false;
	}
	return true;
}

bool File::scan_files(char *path)
{
	int i;
	char *fn;
	
	bool ret = opendir(path);
	if (ret)
	{
		i = strlen(path);
		for (;;)
		{
			//读取目录下的内容，再读会自动读下一个文件
			//为空时表示所有项目读取完毕，跳出
			ret = readdir();
			if (!ret) break;
			fn = getName();
			//点表示当前目录，跳过
			if (*fn == '.') continue;
			//目录，递归读取
			if (isHidden())
			{
				//合成完整目录名
				sprintf(&path[i], "/%s", fn);
				//递归遍历
				ret = scan_files(path);
				path[i] = 0;
				//打开失败，跳出循环
				if (!ret)	break;
			}
			else
			{
				Serial.printf("%s/%s\r\n", path, fn);	//输出文件名
				/* 可以在这里提取特定格式的文件路径 */
			}
		}
	}
	closedir();
	return ret;
}

















