#ifndef __TEXTEDITOR_PRIVATE_H
#define __TEXTEDITOR_PRIVATE_H

#include "Fatfs.h"
//#include "SdFat.h"

#if 0
void TextEditorSet(char* text, SdFile file);
#endif
void TextEditorSet(char* text, File file);

bool TextEditorGetLocked();

#define TextBuffSize (5 * 1024)

#endif
