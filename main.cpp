/**
  ******************************************************************************
  * @file    TIM_PWM_Output/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    19-September-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define TESTRESULT_ADDRESS         0x080FFFFC
#define ALLTEST_PASS               0x00000000
#define ALLTEST_FAIL               0x55555555

#define X_POS 0
#define Y_POS 1
#define Z_POS 2

#define TO_DEGREES 57.29578f

__IO uint8_t UserButtonPressed = 0x00;
__IO uint8_t DemoEnterCondition = 0x00;

__IO uint8_t g_pidEnable = 0;

Motor motorL;
Motor motorR;

PID Bal_pid;
	
LIS302DL_InitTypeDef  LIS302DL_InitStruct;
LIS302DL_FilterConfigTypeDef LIS302DL_FilterStruct;  
__IO uint8_t X_Offset, Y_Offset, Z_Offset  = 0x00;
int32_t Buffer[6];

double roll = 0;
double rollAjust = 0;

// Kalman filter
Kalman kRoll;
double kAngle = 0;
double kAngleRate = 0;
//Kalman kPitch; // not use

// for balancer PID
double Bal_PWM = 0;
double Bal_Setpoint = 0;
double Bal_PWM_IN = 0;

// Alpha Beta Filter
AlphaBetaFilter abf;
double abf_res;


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */
	
	uint32_t tmpTime = 0;
	
	// Initialize lowlevel control functions	
	//
	delay_init();
	//
	motor_init();
	// Encoder Configuration
	Encoders_init();
	Encoders_reset();
	
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);
	
	
	/* MEMS configuration */
	LIS302DL_InitStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;
	LIS302DL_InitStruct.Output_DataRate = LIS302DL_DATARATE_100;
	LIS302DL_InitStruct.Axes_Enable = LIS302DL_XYZ_ENABLE;
	LIS302DL_InitStruct.Full_Scale = LIS302DL_FULLSCALE_2_3;
	LIS302DL_InitStruct.Self_Test = LIS302DL_SELFTEST_NORMAL;
	LIS302DL_Init(&LIS302DL_InitStruct);
	
	/* Required delay for the MEMS Accelerometre: Turn-on time = 3/Output data Rate 
	= 3/100 = 30ms */
	_delay_ms(30);
	
	/* MEMS High Pass Filter configuration */
	LIS302DL_FilterStruct.HighPassFilter_Data_Selection = LIS302DL_FILTEREDDATASELECTION_OUTPUTREGISTER;
	LIS302DL_FilterStruct.HighPassFilter_CutOff_Frequency = LIS302DL_HIGHPASSFILTER_LEVEL_1;
	LIS302DL_FilterStruct.HighPassFilter_Interrupt = LIS302DL_HIGHPASSFILTERINTERRUPT_1_2;
	LIS302DL_FilterConfig(&LIS302DL_FilterStruct);
	
	LIS302DL_ReadACC(Buffer);
	
	roll = atanf(-Buffer[Y_POS] / sqrtf(Buffer[X_POS] * Buffer[X_POS] + 
																						Buffer[Z_POS] * Buffer[Z_POS])) * TO_DEGREES;
	roll += rollAjust;
	kRoll.setAngle(roll);
	
	
	// Alpha Beta Filter
	abf.InitializeAlphaBeta(0, 0.5, 0.1);
	
	
	// Motor Configuration
	motorL.registerControl(&motor_left_pwm, &ENCL_getValue); // the negative value of pwm make direction in reverse
  motorR.registerControl(&motor_right_pwm, &ENCR_getValue);
	
	// change P, I, D if you want
	motorL._pid.SetTunings(1, 10, 0); // P = 1, I = 0, D = 0
	motorR._pid.SetTunings(1, 10, 0); // P = 1, I = 0, D = 0
	
	// change interval time
	motorL.setInterval(1);
	motorR.setInterval(1);
	
	// change output limited
	motorL._pid.SetOutputLimits(-PWM_MAX, PWM_MAX);
	motorR._pid.SetOutputLimits(-PWM_MAX, PWM_MAX);
	
	// set pid enable for each motors
	// if you want to see encoder value, this setting is required
	motorL.PID_Enable = 1;
	motorR.PID_Enable = 1;
	// max speed 103
	
	// Configure Balance PID
	Bal_pid.parameters(&Bal_PWM_IN, &Bal_PWM, &Bal_Setpoint, 0.01, 0.5, 0.0000006, REVERSE);//4.0, 0.08, 0.18, REVERSE);
	Bal_pid.SetMode(AUTOMATIC);
	Bal_pid.SetSampleTime(10);
	Bal_pid.SetOutputLimits(-PWM_MAX, PWM_MAX);

	
	// enable global pid
	g_pidEnable = 1;
	
//	motorL.run(80);
//	motorR.run(80);
//	_delay_ms(1000);
//	
//	motorL.run(-80);
//	motorR.run(-80);
//	_delay_ms(1000);
//	
//	motorL.run(0);
//	motorR.run(0);
//	_delay_ms(2000);
	
	tmpTime = millis();	
  
	while (1)
  {
		motorL.run(((double)52237.6)*sinf(Bal_PWM/TO_DEGREES));	// motor pid setpoint
		motorR.run(((double)52237.6)*sinf(Bal_PWM/TO_DEGREES));	// motor pid setpoint
		
		_delay_ms(1);					
		
		if (STM_EVAL_PBGetState(BUTTON_USER)) // set new offset
		{
			_delay_ms(10);
			LIS302DL_ReadACC(Buffer);
			roll = atanf(-Buffer[Y_POS] / sqrtf(Buffer[X_POS] * Buffer[X_POS] + 
																						Buffer[Z_POS] * Buffer[Z_POS])) * TO_DEGREES;
			rollAjust = 0 - roll;
		}
		

		LIS302DL_ReadACC(Buffer);
			
		roll = atanf(-Buffer[Y_POS] / sqrtf(Buffer[X_POS] * Buffer[X_POS] + 
																						Buffer[Z_POS] * Buffer[Z_POS])) * TO_DEGREES;
		roll += rollAjust;
		
//		roll *= 2;
			
		
		kAngle = kRoll.getAngle(roll, 1000, (double)0.001);
		
		Bal_PWM_IN = kAngle; //((double)1701.049/10)*sinf(kAngle/TO_DEGREES); // ENC/ms - real speed
	}
}


void TimeOutProcess(void)
{
	STM_EVAL_LEDToggle(LED4);
}


/**
  * @brief  This function handles the test program fail.
  * @param  None
  * @retval None
  */
void Fail_Handler(void)
{
  /* Erase last sector */ 
  FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3);
  /* Write FAIL code at last word in the flash memory */
  FLASH_ProgramWord(TESTRESULT_ADDRESS, ALLTEST_FAIL);
  
  while(1)
  {
    /* Toggle Red LED */
    STM_EVAL_LEDToggle(LED5);
    _delay_ms(5);
  }
}


/**
  * @brief  MEMS accelerometre management of the timeout situation.
  * @param  None.
  * @retval None.
  */
uint32_t LIS302DL_TIMEOUT_UserCallback(void)
{
  /* MEMS Accelerometer Timeout error occured during Test program execution */
  if (DemoEnterCondition == 0x00)
  {
    /* Timeout error occured for SPI TXE/RXNE flags waiting loops.*/
    Fail_Handler();    
  }
  /* MEMS Accelerometer Timeout error occured during Demo execution */
  else
  {
    while (1)
    {   
    }
  }
  return 0;  
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  while (1)
  {}
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
