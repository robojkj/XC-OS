#include "EEPROM_File.h"

#include "Fatfs.h"

static File file;


bool EEPROM_File::begin(const char* dir, const char* fileName, uint32_t size)
{
	if (file.exists(dir))
	{
		f_mkdir(dir);
	}
	
	String path = String(dir) + "/" + String(fileName);
	bool res = file.open(path.c_str(), FA_READ | FA_WRITE | FA_CREATE_NEW);
	if (res)
	{
		while (file.getSize() < size)
		{
			file.write((uint8_t)0);
		}
	}
	return res;
#if 0
    if(!SD.exists(dir))
    {
        SD.mkdir(dir);
    }
    String path = String(dir) + "/" + String(fileName);
    bool retval = file.open(path.c_str(), O_RDWR | O_CREAT);
    if(retval)
    {
        while(file.fileSize() < size)
        {
            file.write((uint8_t)0);
        }
    }
	return retval;
#endif
    
}

void EEPROM_File::end()
{
#if 0
    file.close();
#endif
	file.close();
}
    
uint8_t EEPROM_File::read(uint32_t idx)
{
    uint8_t retval = 0;

	if (file.isOpen() && idx < file.getSize())
	{
		file.seekSet(idx);
		file.read(&retval, 1);
	}
	
#if 0
    if(file.isOpen() && idx < file.fileSize())
    {
        file.seekSet(idx);
        retval = file.read();
    }
#endif 
    return retval;
}

void EEPROM_File::write(uint32_t idx, uint8_t val)
{
#if 0
    if(file.isOpen() && idx < file.fileSize())
    {
	
        file.seekSet(idx);
        file.write(val);
    }
#endif
	if (file.isOpen() && idx < file.getSize())
	{
		file.seekSet(idx);
		file.write(val);
	}
}

void EEPROM_File::update(uint32_t idx, uint8_t val)
{
    if(read(idx) != val)
    {
        write(idx, val);
    }
}




