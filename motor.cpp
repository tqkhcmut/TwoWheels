#include "motor.h"

Motor::Motor(void)
{
	PID_P = 0;
	PID_I = 0;
	PID_D = 0;
	
	
	PWM_Val = 0.0;
	ENC_Val = 0.0;
	SetPoint = 0.0;
	
	interval = 10;
	lastTime = millis();
//	gDirective = 0;
	
	_pid = PID();
	_pid.parameters(&ENC_Val, &PWM_Val, &SetPoint, PID_P, PID_I, PID_D, DIRECT);
	_pid.SetMode(AUTOMATIC);
	_pid.SetSampleTime(interval);
	
	PID_Enable = 0;	
		
	Encoder_Value = 0;
	preEncoder_Value = 0;
		
}

void Motor::setInterval(unsigned int _interval)
{
	interval = _interval;
	_pid.SetSampleTime(interval);
	lastTime = millis();
}

void Motor::registerControl(PWMCtl __pwm, getEncoder __getEncoder)
{
	pwm = __pwm;
//	dir = edir;
	gEncoder = __getEncoder;
}

void Motor::ping(void)
{
	if (PID_Enable)
	{
		// PID service here
		
		
		// we need to convert encoder value to pwm input
		// we have 334 encoder pulse per round
		// and 4300 rpm at 12V or PWM_MAX = 3000
		// after calculation, we have 23928 pulse per second with PWM_MAX value
		
		if (millis() - lastTime >= interval)
		{
			lastTime = millis();
			Encoder_Value += (*gEncoder)();
			ENC_Val = (Encoder_Value - preEncoder_Value);// * 125.3234 / interval; // * 1000 * 3000 / (interval * 23938);
			preEncoder_Value = Encoder_Value;
		}
		_pid.Compute();
		
		// we need to convert ENC per second to pwm value
		// PWM_Val is number ENC per interval
		
		(*pwm)((int)(PWM_Val* 125.3234 / interval));
	}
}
void Motor::run(int speed)
{
//	gDirective = directive;

	if (PID_Enable)
	{
		SetPoint = (double) speed;
	}
	else
	{
		SetPoint = (double) speed;
		(*pwm)(speed);
	}
}	

void Motor::run(int speed, int time)
{
	unsigned int capTime = millis();
	
//	gDirective = directive;

	if (PID_Enable)
	{
		SetPoint = (double) speed;
		
		while(millis() - capTime < time);
		
		(*pwm)(0);
	}
	else
	{
		SetPoint = (double) speed;
		(*pwm)(speed);
		
		while(millis() - capTime < time);
		
		(*pwm)(0);
	}
}

void Motor::run(int speed, unsigned int numberOfEncoderPulse)
{
	unsigned int capEncoder = Encoder_Value;
//	gDirective = directive;

	if (PID_Enable)
	{
		SetPoint = (double) speed;
		
		while(Encoder_Value - capEncoder < numberOfEncoderPulse);
		
		(*pwm)(0);
	}
	else
	{
		
		SetPoint = (double) speed;
		(*pwm)(speed);
		
		while(Encoder_Value - capEncoder < numberOfEncoderPulse);
		
		(*pwm)(0);
	}
}
