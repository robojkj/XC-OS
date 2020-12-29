#include "Basic/FileGroup.h"
#include "Basic/TasksManage.h"
#include "BMP280_DEV.h"
#include "ClosedCube_SHT31D.h"
#include "BSP.h"
#include "Wire.h"

static ClosedCube_SHT31D sht3xd;

static float temperature, pressure, altitude;	//创建温度、压力和高度变量
BMP280_DEV bmp280;  //实例化（创建）BMP280_DEV对象并设置I2C操作


void printResult(String text, SHT31D result)
{
	if (result.error == SHT3XD_NO_ERROR) {
		Serial.print(text);
		Serial.print(": T=");
		Serial.print(result.t);
		Serial.print("C, RH=");
		Serial.print(result.rh);
		Serial.println("%");
	} else {
		Serial.print(text);
		Serial.print(": [ERROR] Code #");
		Serial.println(result.error);
	}
}

void init_sensor(void)
{
	sht3xd.begin(0x44); // I2C address: 0x44 or 0x45

	//Serial.print("Serial #");
	//Serial.println(sht3xd.readSerialNumber());

	bmp280.begin(BMP280_I2C_ALT_ADDR);  // 使用可选I2C地址（0x76）进行默认初始化，将BMP280置于休眠模式
	bmp280.setPresOversampling(OVERSAMPLING_X4);    // 将压力过采样设置为X4
	bmp280.setTempOversampling(OVERSAMPLING_X1);    // 将温度过采样设置为X1
	bmp280.setIIRFilter(IIR_FILTER_4);             // 将IIR滤波器设置为设置4
	bmp280.setTimeStandby(TIME_STANDBY_1000MS);    // 将待机时间设置为1秒
	bmp280.startNormalConversion();                // 在正常模式下启动BMP280连续转换
}


void Task_SensorInfoUpdate(TimerHandle_t xTimer)
{
	__ExecuteOnce((
		//delay(100),
		init_sensor()
    ));
	
	Wire.Scanner();
	
	/*
	if (sht3xd.periodicStart(SHT3XD_REPEATABILITY_HIGH, SHT3XD_FREQUENCY_10HZ) != SHT3XD_NO_ERROR)
		Serial.println("[ERROR] Cannot start periodic mode");
	
	printResult("Periodic Mode", sht3xd.periodicFetchData());
	
	//唤醒BMP280，提出测量要求
	bmp280.startForcedConversion();
	//获取结果并输出
	if (bmp280.getMeasurements(temperature, pressure, altitude))
	{
		Serial.print(temperature - 2);
		Serial.print(F("*C   "));
		Serial.print(pressure * 98);
		Serial.print(F("hPa   "));
		Serial.print(altitude + 143);
		Serial.println(F("m"));
	}
	*/
	
	/*
	__ExecuteOnce((
		Audio_SetEnable(true),
		Audio_ToneSetVolume(500)
	));
	Audio_Tone(1000, 500);
	*/
}








