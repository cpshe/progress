#include	"./SYSTEM/sys/sys.h"
#include	"./BSP/SERVO/servo.h"
#include	"./BSP/PWMTIMER/pwmtimer.h"

void Servo_Init(void)
{
	PWM_Init();
}

void Servo_SetAngle(float Angle)
{
	PWM_SetCompare1(Angle/180*2000+500);
}
