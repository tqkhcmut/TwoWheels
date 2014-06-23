#ifndef motor_h
#define motor_h

#ifdef __cplusplus
extern "C" {
#endif
	
	#include "PID_v1.h"
	#include "delay.h"
	#include <inttypes.h>
	
	typedef void (*PWMCtl) (int);	// control speed of motor
//	typedef void (*DIRCtl) (unsigned char); // 1: forward; 0: reverse
	typedef int16_t (*getEncoder) (void); // get value of encoder

	class Motor
	{
		private:
			// PID 
			double PID_P;
			double PID_I;
			double PID_D;
			
			double PWM_Val;
			double ENC_Val;
			double SetPoint; // the value want to
			
			
			int Encoder_Value;
			int preEncoder_Value;
			
			PWMCtl pwm; // control speed of motor
//			DIRCtl dir;	// control directive of motor
			getEncoder gEncoder; // get value of encoder
		
//			unsigned char gDirective; // capture of the direction
			
			unsigned int lastTime;
			unsigned int interval; //
			
		public:
			
			unsigned char PID_Enable;
			
			PID _pid;
			Motor(void);
			void registerControl(PWMCtl __pwm, getEncoder __getEncoder); 
			void setInterval(unsigned int _interval);
		void ping(void);
//			void run(int speed, unsigned char directive); // 
//			void run(int speed, unsigned char directive, int time);
//			void run(int speed, unsigned char directive, unsigned int numberOfEncoderPulse);
			
			void run(int speed); // 
			void run(int speed, int time);
			void run(int speed, unsigned int numberOfEncoderPulse);
	}; 	

#ifdef __cplusplus
}
#endif

#endif

