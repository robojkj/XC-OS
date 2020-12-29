#include "Basic/FileGroup.h"
#include "Basic/TasksManage.h"
#include "BSP.h"

static bool State_MotorVibrate = true;
static uint32_t MotorStop_TimePoint = 0;
static bool IsMotorRunning = false;

static uint8_t PWM_PIN;

static void Init_Motor()
{
	uint8_t temp;
	if(Motor_DIR)
	{
		PWM_PIN = Motor_IN1_Pin;
		temp = Motor_IN2_Pin;
	}else
	{
		PWM_PIN = Motor_IN2_Pin;
		temp = Motor_IN1_Pin;
	}
	
    PWM_Init(PWM_PIN, 1000, 80);
	pinMode(temp, OUTPUT); 
	pinMode(Motor_SLP_Pin, OUTPUT);
	
	digitalWrite(temp, LOW);
	digitalWrite(Motor_SLP_Pin, HIGH);
	
    Motor_Vibrate(0.9f, 1000);
}

void Task_MotorRunning(TimerHandle_t xTimer)
{
    __ExecuteOnce(Init_Motor());
    
    if(IsMotorRunning && millis() >= MotorStop_TimePoint)
    {
        analogWrite(PWM_PIN, 0);
		digitalWrite(Motor_SLP_Pin, LOW);
        IsMotorRunning = false;
    }
}

void Motor_SetEnable(bool en)
{
    State_MotorVibrate = en;
}

void Motor_Vibrate(float strength, uint32_t time)
{
   if(!State_MotorVibrate)
       return;
   
   __LimitValue(strength, 0.0f, 1.0f);
   
   digitalWrite(Motor_SLP_Pin, HIGH);
   analogWrite(PWM_PIN, strength * 1000);
   IsMotorRunning = true;
   MotorStop_TimePoint = millis() + time;
}

void Motor_SetState(bool state)
{
    if(!State_MotorVibrate)
       return;
    
    analogWrite(PWM_PIN, state ? 1000 : 0);
}

