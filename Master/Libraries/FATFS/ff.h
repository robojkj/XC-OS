/*---------------------------------------------------------------------------/
/  FatFs - FAT file system module include R0.11a    (C)ChaN, 2015
/----------------------------------------------------------------------------/
/ FatFs module is a free software that opened under license policy of
/ following conditions.
/
/ Copyright (C) 2015, ChaN, all right reserved.
/
/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/---------------------------------------------------------------------------*/


#ifndef _FATFS
#define _FATFS	64180	/* Revision ID */

#ifdef __cplusplus
extern "C" {
#endif

#include "integer.h"	/* Basic integer types */
#include "ffconf.h"		/* FatFs configuration options */
#if _FATFS != _FFCONF
#error Wrong configuration file (ffconf.h).
#endif



/* Definitions of volume management */

#if _MULTI_PARTITION		/* Multiple partition configuration */
typedef struct {
	BYTE pd;	/* Physical drive number */
	BYTE pt;	/* Partition: 0:Auto detect, 1-4:Forced partition) */
} PARTITION;
extern PARTITION VolToPart[];	/* Volume - Partition resolution table */
#define LD2PD(vol) (VolToPart[vol].pd)	/* Get physical drive number */
#define LD2PT(vol) (VolToPart[vol].pt)	/* Get partition index */

#else							/* Single partition configuration */
#define LD2PD(vol) (BYTE)(vol)	/* Each logical drive is bound to the same physical drive number */
#define LD2PT(vol) 0			/* Find first valid partition or in SFD */

#endif



/* Type of path name strings on FatFs API */

#if _LFN_UNICODE			/* Unicode string */
#if !_USE_LFN
#error _LFN_UNICODE must be 0 at non-LFN cfg.
#endif
#ifndef _INC_TCHAR
typedef WCHAR TCHAR;
#define _T(x) L ## x
#define _TEXT(x) L ## x
#endif

#else						/* ANSI/OEM string */
#ifndef _INC_TCHAR
typedef char TCHAR;
#define _T(x) x
#define _TEXT(x) x
#endif

#endif



/* File system object structure (FATFS) */

typedef struct {
	BYTE	fs_type;		/* FAT sub-type (0:Not mounted) FAT子类型(0:未挂载) */
	BYTE	drv;			/* Physical drive number 物理驱动器号 */
	BYTE	csize;			/* Sectors per cluster (1,2,4...128) 每个集群的扇区(1,2,4…128) */
	BYTE	n_fats;			/* Number of FAT copies (1 or 2) 拷贝数(1或2) */
	BYTE	wflag;			/* win[] flag (b0:dirty) */
	BYTE	fsi_flag;		/* FSINFO flags (b7:disabled, b0:dirty) */
	WORD	id;				/* File system mount ID 文件系统挂载ID */
	WORD	n_rootdir;		/* Number of root directory entries (FAT12/16) 根目录项数(FAT12/16)  */
#if _MAX_SS != _MIN_SS
	WORD	ssize;			/* Bytes per sector (512, 1024, 2048 or 4096) 每个扇区字节数(512、1024、2048或4096) */
#endif
#if _FS_REENTRANT
	_SYNC_t	sobj;			/* Identifier of sync object */
#endif
#if !_FS_READONLY
	DWORD	last_clust;		/* Last allocated cluster 最近分配的集群 */
	DWORD	free_clust;		/* Number of free clusters 空闲集群数量 */
#endif
#if _FS_RPATH
	DWORD	cdir;			/* Current directory start cluster (0:root) */
#endif
	DWORD	n_fatent;		/* Number of FAT entries, = number of clusters + 2 FAT条目数量，=集群数量+ 2 */
	DWORD	fsize;			/* Sectors per FAT 扇区 */
	DWORD	volbase;		/* Volume start sector 卷启动扇区*/
	DWORD	fatbase;		/* FAT start sector FAT start扇区 */
	DWORD	dirbase;		/* Root directory start sector (FAT32:Cluster#) 根目录启动扇区(FAT32:Cluster#)  */
	DWORD	database;		/* Data start sector 数据启动扇区 */
	DWORD	winsect;		/* Current sector appearing in the win[] 当前扇区出现在win[] */
	BYTE	win[_MAX_SS];	/* Disk access window for Directory, FAT (and file data at tiny cfg) 用于目录、FAT(和极小cfg上的文件数据)的磁盘访问窗口 */
} FATFS;



/* File object structure (FIL) */

typedef struct {
	FATFS*	fs;				/* Pointer to the related file system object (**do not change order**) 指向相关文件系统对象的指针(**不更改顺序**) */
	WORD	id;				/* Owner file system mount ID (**do not change order**) 所有者文件系统挂载ID(**不更改订单**) */
	BYTE	flag;			/* Status flags 状态标志 */
	BYTE	err;			/* Abort flag (error code) 终止标志(错误代码) */
	DWORD	fptr;			/* File read/write pointer (Zeroed on file open) 文件读/写指针(文件打开时为零) */
	DWORD	fsize;			/* File size 文件大小 */
	DWORD	sclust;			/* File start cluster (0:no cluster chain, always 0 when fsize is 0) 文件启动集群(0:无集群链，fsize为0时始终为0) */
	DWORD	clust;			/* Current cluster of fpter (not valid when fprt is 0) fpter的当前集群(fprt为0时无效) */
	DWORD	dsect;			/* Sector number appearing in buf[] (0:invalid) 扇区编号出现在buf[](0:无效) */
#if !_FS_READONLY
	DWORD	dir_sect;		/* Sector number containing the directory entry 包含目录项的扇区编号  */
	BYTE*	dir_ptr;		/* Pointer to the directory entry in the win[] 指向win[] 中 的目录条目的指针 */
#endif
#if _USE_FASTSEEK
	DWORD*	cltbl;			/* Pointer to the cluster link map table (Nulled on file open) 指向集群链接映射表的指针(文件打开时为空) */
#endif
#if _FS_LOCK
	UINT	lockid;			/* File lock ID origin from 1 (index of file semaphore table Files[]) */
#endif
#if !_FS_TINY
	BYTE	buf[_MAX_SS];	/* File private data read/write window 字节缓冲区(_MAX_SS);文件私有数据读/写窗口*/
#endif
} FIL;



/* Directory object structure (DIR) */

typedef struct {
	FATFS*	fs;				/* Pointer to the owner file system object (**do not change order**) 指向所有者文件系统对象(**不更改顺序**)的指针 */
	WORD	id;				/* Owner file system mount ID (**do not change order**) 所有者文件系统挂载ID(**不更改订单**) */
	WORD	index;			/* Current read/write index number 当前读/写索引号 */
	DWORD	sclust;			/* Table start cluster (0:Root dir) 表启动集群(0:根目录) */
	DWORD	clust;			/* Current cluster 当前集群 */
	DWORD	sect;			/* Current sector 当前扇区*/
	BYTE*	dir;			/* Pointer to the current SFN entry in the win[]	指向win[] 中 当前SFN条目的指针 */
	BYTE*	fn;				/* Pointer to the SFN (in/out) {file[8],ext[3],status[1]} */
#if _FS_LOCK
	UINT	lockid;			/* File lock ID (index of file semaphore table Files[]) */
#endif
#if _USE_LFN
	WCHAR*	lfn;			/* Pointer to the LFN working buffer */
	WORD	lfn_idx;		/* Last matched LFN index number (0xFFFF:No LFN) */
#endif
#if _USE_FIND
	const TCHAR*	pat;	/* Pointer to the name matching pattern */
#endif
} DIR;



/* File information structure (FILINFO) */

typedef struct {
	DWORD	fsize;			/* File size	文件大小 */
	WORD	fdate;			/* Last modified date	上次修改日期 */
	WORD	ftime;			/* Last modified time	上次修改时间 */
	BYTE	fattrib;		/* Attribute		属性 */
	TCHAR	fname[13];		/* Short file name (8.3 format)		短文件名(8.3格式) */
#if _USE_LFN
	TCHAR*	lfname;			/* Pointer to the LFN buffer		指向LFN缓冲区的指针 */
	UINT 	lfsize;			/* Size of LFN buffer in TCHAR		TCHAR中LFN缓冲区的大小 */
#endif
} FILINFO;



/* File function return code (FRESULT) */

typedef enum {
	FR_OK = 0,				/* (0) Succeeded 成功 */
	FR_DISK_ERR,			/* (1) A hard error occurred in the low level disk I/O layer 在低级别磁盘I/O层发生了一个硬错误 */
	FR_INT_ERR,				/* (2) Assertion failed 断言失败 */
	FR_NOT_READY,			/* (3) The physical drive cannot work 物理驱动器不能工作 */
	FR_NO_FILE,				/* (4) Could not find the file 找不到文件 */
	FR_NO_PATH,				/* (5) Could not find the path 找不到路径 */
	FR_INVALID_NAME,		/* (6) The path name format is invalid 路径名格式无效 */
	FR_DENIED,				/* (7) Access denied due to prohibited access or directory full 由于被禁止访问或目录已满而拒绝访问 */
	FR_EXIST,				/* (8) Access denied due to prohibited access 由于禁止访问而拒绝访问 */
	FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid 文件/目录对象无效 */
	FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected 物理驱动器是写保护的 */
	FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid 逻辑驱动器号无效 */
	FR_NOT_ENABLED,			/* (12) The volume has no work area 卷没有工作区域 */
	FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume 没有有效的FAT卷 */
	FR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any parameter error 由于参数错误而中止 */
	FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period 无法获得在定义的时间段内访问卷的授权 */
	FR_LOCKED,				/* (16) The operation is rejected according to the file sharing policy 根据文件共享策略拒绝操作 */
	FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated  LFN工作缓冲区不能被分配 */
	FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_LOCK 打开的文件数量大于_FS_LOCK */
	FR_INVALID_PARAMETER	/* (19) Given parameter is invalid 给定参数无效 */
} FRESULT;



/*--------------------------------------------------------------*/
/* FatFs module application interface                           */

FRESULT f_open (FIL* fp, const TCHAR* path, BYTE mode);				/* Open or create a file 打开或创建一个文件 */
FRESULT f_close (FIL* fp);											/* Close an open file object 关闭打开的文件对象 */
FRESULT f_read (FIL* fp, void* buff, UINT btr, UINT* br);			/* Read data from a file 从文件中读取数据 */
FRESULT f_write (FIL* fp, const void* buff, UINT btw, UINT* bw);	/* Write data to a file 将数据写入文件 */
FRESULT f_forward (FIL* fp, UINT(*func)(const BYTE*,UINT), UINT btf, UINT* bf);	/* Forward data to the stream 将数据转发到流 */
FRESULT f_lseek (FIL* fp, DWORD ofs);								/* Move file pointer of a file object 移动文件对象的文件指针 */
FRESULT f_truncate (FIL* fp);										/* Truncate file 截断文件 */
FRESULT f_sync (FIL* fp);											/* Flush cached data of a writing file 刷新写入文件的缓存数据 */
FRESULT f_opendir (DIR* dp, const TCHAR* path);						/* Open a directory 打开一个目录 */
FRESULT f_closedir (DIR* dp);										/* Close an open directory 关闭打开的目录 */
FRESULT f_readdir (DIR* dp, FILINFO* fno);							/* Read a directory item 读取一个目录项 */
FRESULT f_findfirst (DIR* dp, FILINFO* fno, const TCHAR* path, const TCHAR* pattern);	/* Find first file 查找第一个文件 */
FRESULT f_findnext (DIR* dp, FILINFO* fno);							/* Find next file Find next 查找下一个文件 */
FRESULT f_mkdir (const TCHAR* path);								/* Create a sub directory 创建子目录 */
FRESULT f_unlink (const TCHAR* path);								/* Delete an existing file or directory 删除现有的文件或目录 */
FRESULT f_rename (const TCHAR* path_old, const TCHAR* path_new);	/* Rename/Move a file or directory 重命名/移动文件或目录 */
FRESULT f_stat (const TCHAR* path, FILINFO* fno);					/* Get file status 得到文件状态 */
FRESULT f_chmod (const TCHAR* path, BYTE attr, BYTE mask);			/* Change attribute of the file/dir 更改文件/目录的属性 */
FRESULT f_utime (const TCHAR* path, const FILINFO* fno);			/* Change times-tamp of the file/dir 更改时间-夯实文件/目录 */
FRESULT f_chdir (const TCHAR* path);								/* Change current directory 改变当前目录 */
FRESULT f_chdrive (const TCHAR* path);								/* Change current drive 改变当前驱动器 */
FRESULT f_getcwd (TCHAR* buff, UINT len);							/* Get current directory 获取当前目录 */
FRESULT f_getfree (const TCHAR* path, DWORD* nclst, FATFS** fatfs);	/* Get number of free clusters on the drive 获取驱动器上空闲集群的数量 */
FRESULT f_getlabel (const TCHAR* path, TCHAR* label, DWORD* vsn);	/* Get volume label 获取磁盘卷标 */
FRESULT f_setlabel (const TCHAR* label);							/* Set volume label 设置卷标 */
FRESULT f_mount (FATFS* fs, const TCHAR* path, BYTE opt);			/* Mount/Unmount a logical drive 挂载/卸载逻辑驱动器 */
FRESULT f_mkfs (const TCHAR* path, BYTE sfd, UINT au);				/* Create a file system on the volume 在卷上创建一个文件系统 */
FRESULT f_fdisk (BYTE pdrv, const DWORD szt[], void* work);			/* Divide a physical drive into some partitions 将物理驱动器划分为若干分区 */
int f_putc (TCHAR c, FIL* fp);										/* Put a character to the file 将一个字符放入文件中 */
int f_puts (const TCHAR* str, FIL* cp);								/* Put a string to the file 在文件中添加一个字符串 */
int f_printf (FIL* fp, const TCHAR* str, ...);						/* Put a formatted string to the file 将格式化字符串放入文件中 */
TCHAR* f_gets (TCHAR* buff, int len, FIL* fp);						/* Get a string from the file 从文件中获取一个字符串 */

	/*
		FatFS软件包提供的API函数

		1、f_mount:注册/注销一个工作区

		2、f_open:打开/创建一个文件

		3、f_close:关闭一个文件

		4、f_read:读文件

		5、f_write:写文件

		6、f_lseek:移动读/写指针，扩展文件大小

		7、f_truncate:截断文件大小

		8、f_sync:刷新缓冲区

		9、f_opendir:打开一个目录

		10、f_readdir:读取目录

		11、f_getfree:获取空闲簇

		12、f_stat:获取文件状态

		13、f_mkdir:创建一个新目录

		14、f_unlink:删除一个文件

		15、f_chmod:改变一个文件或目录的属性

		16、f_utime:改变一个文件或目录的时间戳

		17、f_rename:重命名一个对象

		18、f_mkfs:格式化

		19、f_forward:读取文件数据转移到数据流设备

		20、f_chdir:改变驱动器的当前目录

		21、f_chdrive:改变当前驱动器

		22、f_getcwd:检索当前目录

		23、f_gets:从文件中读取字符串

		24、f_putc:从文件中写一个字符

		25、f_puts:往文件中写一个字符串

		26、f_printf:往文件中写入格式化字符串
	*/
	
	
	
	
	
	
	
#define f_eof(fp) ((int)((fp)->fptr == (fp)->fsize))
#define f_error(fp) ((fp)->err)
#define f_tell(fp) ((fp)->fptr)
#define f_size(fp) ((fp)->fsize)
#define f_rewind(fp) f_lseek((fp), 0)
#define f_rewinddir(dp) f_readdir((dp), 0)

#ifndef EOF
#define EOF (-1)
#endif




/*--------------------------------------------------------------*/
/* Additional user defined functions                            */

/* RTC function */
#if !_FS_READONLY && !_FS_NORTC
DWORD get_fattime (void);
#endif

/* Unicode support functions */
#if _USE_LFN							/* Unicode - OEM code conversion */
WCHAR ff_convert (WCHAR chr, UINT dir);	/* OEM-Unicode bidirectional conversion */
WCHAR ff_wtoupper (WCHAR chr);			/* Unicode upper-case conversion */
#if _USE_LFN == 3						/* Memory functions */
void* ff_memalloc (UINT msize);			/* Allocate memory block */
void ff_memfree (void* mblock);			/* Free memory block */
#endif
#endif

/* Sync functions */
#if _FS_REENTRANT
int ff_cre_syncobj (BYTE vol, _SYNC_t* sobj);	/* Create a sync object */
int ff_req_grant (_SYNC_t sobj);				/* Lock sync object */
void ff_rel_grant (_SYNC_t sobj);				/* Unlock sync object */
int ff_del_syncobj (_SYNC_t sobj);				/* Delete a sync object */
#endif




/*--------------------------------------------------------------*/
/* Flags and offset address                                     */


/* File access control and file status flags (FIL.flag) */

#define	FA_READ				0x01	/* 读模式 */	
#define	FA_OPEN_EXISTING	0x00	/* 默认打开方式 */

#if !_FS_READONLY
#define	FA_WRITE			0x02	/* 写模式 */
#define	FA_CREATE_NEW		0x04	/* 新建文件，如果文件已存在，则新建失败 */
#define	FA_CREATE_ALWAYS	0x08	/* 新建文件，如果文件已存在，覆盖旧文件 */
#define	FA_OPEN_ALWAYS		0x10	/* 打开文件，如果文件不存在，则创建一个新文件   用此种方式，可以用 f_lseek 在文件后追加数据*/
#define FA__WRITTEN			0x20
#define FA__DIRTY			0x40
#endif


/* FAT sub type (FATFS.fs_type) */

#define FS_FAT12	1
#define FS_FAT16	2
#define FS_FAT32	3


/* File attribute bits for directory entry */

#define	AM_RDO	0x01	/* Read only 只读文件 */
#define	AM_HID	0x02	/* Hidden  隐藏文件 */
#define	AM_SYS	0x04	/* System 系统文件 */
#define	AM_VOL	0x08	/* Volume label 卷标文件 */
#define AM_LFN	0x0F	/* LFN entry */
#define AM_DIR	0x10	/* Directory 程序目录 */
#define AM_ARC	0x20	/* Archive 存档文件 */
#define AM_MASK	0x3F	/* Mask of defined bits */


/* Fast seek feature */
#define CREATE_LINKMAP	0xFFFFFFFF



/*--------------------------------*/
/* Multi-byte word access macros  */

#if _WORD_ACCESS == 1	/* Enable word access to the FAT structure */
#define	LD_WORD(ptr)		(WORD)(*(WORD*)(BYTE*)(ptr))
#define	LD_DWORD(ptr)		(DWORD)(*(DWORD*)(BYTE*)(ptr))
#define	ST_WORD(ptr,val)	*(WORD*)(BYTE*)(ptr)=(WORD)(val)
#define	ST_DWORD(ptr,val)	*(DWORD*)(BYTE*)(ptr)=(DWORD)(val)
#else					/* Use byte-by-byte access to the FAT structure */
#define	LD_WORD(ptr)		(WORD)(((WORD)*((BYTE*)(ptr)+1)<<8)|(WORD)*(BYTE*)(ptr))
#define	LD_DWORD(ptr)		(DWORD)(((DWORD)*((BYTE*)(ptr)+3)<<24)|((DWORD)*((BYTE*)(ptr)+2)<<16)|((WORD)*((BYTE*)(ptr)+1)<<8)|*(BYTE*)(ptr))
#define	ST_WORD(ptr,val)	*(BYTE*)(ptr)=(BYTE)(val); *((BYTE*)(ptr)+1)=(BYTE)((WORD)(val)>>8)
#define	ST_DWORD(ptr,val)	*(BYTE*)(ptr)=(BYTE)(val); *((BYTE*)(ptr)+1)=(BYTE)((WORD)(val)>>8); *((BYTE*)(ptr)+2)=(BYTE)((DWORD)(val)>>16); *((BYTE*)(ptr)+3)=(BYTE)((DWORD)(val)>>24)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _FATFS */
