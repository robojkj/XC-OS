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


#ifndef _FATFS_TEST
#define _FATFS_TEST

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*printf_cb_t)(const char *__restrict __format, ...);
	
void FATFS_TEST(printf_cb_t cb);
	
	
#ifdef __cplusplus
}
#endif

#endif /* _FATFS */
