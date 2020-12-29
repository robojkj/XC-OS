#ifndef _FATFS_H_
#define _FATFS_H_

#include "Arduino.h"
#include "ff.h"

class Fatfs {
public:
	
	Fatfs(uint8_t id);
	bool init();
	bool close();
	bool getSize(uint32_t* cardSize, uint32_t* volFree);
	bool mkfs();
	bool getlabel(char *path);
	bool setlabel(char *path);
	FRESULT get_error();

private:
	FATFS fs;	/* FatFs文件系统对象 */
	char _id[2];
	FRESULT	res;

};

class File {
public:
	
	File();
	bool open(const char *path, uint8_t mode);
	bool close();
	bool isOpen();
	int write(uint8_t c);
	int write(const TCHAR* c);
	int write(const void* buff, uint32_t Size);
	int printf(const char * format, ...);
	int read();
	int read(void* buff, uint32_t Size);
	String readStringUntil(char terminator);
	FRESULT get_error();
	bool seekSet(uint64_t offset);
	uint32_t tell();
	uint32_t getSize();
	char* getName();
	uint32_t getDate();
	uint32_t getTime();
	uint32_t getAttrib();
	bool state(const TCHAR* path);
	bool unlink(const char *path);
	bool rename(const char *oldname, const char* newname);
	bool exists(const char *path);
	bool mkdir(const char* path);
	bool opendir(const char *path);
	bool closedir();
	bool readdir();
	bool scan_files(char *path);
	
	bool isDir() const {
		return cur_fno.fattrib & AM_DIR;
	}

	bool isFile() const {
		return cur_fno.fattrib & (AM_VOL | AM_ARC);
	}

	bool isHidden() const {
		return cur_fno.fattrib & AM_HID;
	}

	bool isLFN() const {
		return cur_fno.fattrib & AM_LFN;
	}

	bool isReadOnly() const {
		return cur_fno.fattrib & AM_RDO;
	}

	bool isSystem() const {
		return cur_fno.fattrib & AM_SYS;
	}

	
	
private:
	void *file;		/* 文件对象 */
	void *dir_obj;	/* 目录对象 */
	FRESULT	res;	/* 公共错误信息反馈 */
	FILINFO cur_fno;	/* 当前文件信息 */
	uint32_t _startMillis;
};





#endif











