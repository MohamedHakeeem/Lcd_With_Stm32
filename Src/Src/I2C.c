
#include "I2C.h"
//#include <stddef.h>
#include "Systick.h"
#include "Gpio.h"


/* At [A0 A1 A2] HAVE  LOW LEVEL   in proteus */
#define address_board 0x20


/* Enable Bit */
#define ENABLE 0x04

/*  Write Bit */
#define RW 0x0

/* data Register Select Bit  /  0x00 instruction register */
#define RS 0x01

void Lcd_Init(void)
{
  /* initialization from data sheet */

	/*wait more than 15ms */
	SysTick_Delay(20);
	Write_4bits(0x03 << 4);
	/*wait more than 4.1 ms */
	SysTick_Delay(10);
	Write_4bits(0x03 << 4);
	/*wait more than 100us */
	SysTick_Delay(1);
	Write_4bits(0x03 << 4);
	Write_4bits(0x02 << 4);

    /*display on & cursor on & blinking on*/
	 send_cmd(0x0F);

	 /*clear display*/
	 send_cmd(0x01);

	 /* 4bits operation & 2lines & number of dots*/
	 send_cmd(0x28);

	 /*entry mode  as cursor increment by 1  and display not follow*/
	 send_cmd(0x06);

	 /*return home */
	 send_cmd(0x02);

}

void set_cursor(uint8_t row, uint8_t col)
{
		/* row & col start from 0 index */
 /* use set DDRAM address for positioning cursor to desired position
  *command for set address in DDRAM  [1 ADD ADD ADD  ADD ADD ADD ADD]-----> [0X80+COL]
  * EACH LINE HAVE RANGE FROM [0---40]
  * */
	if(row==0)
	{
		send_cmd(0X80 | col);

	}

	else if(row==1)
	{
		send_cmd(0xC0 | col);

	}

	else{


		//do nothing
	}

}


void send_string(char *str )
{
		for(int i=0; (str[i] != '\0') ;i++)
		{

				send_ch(str[i]);
		}

}


void send_cmd(uint8_t cmd)
{
	send(cmd ,  0 );

}

void send_ch(uint8_t letter)
{
	send(letter ,  RS );

}

void send(uint8_t letter , uint8_t mode)
{
	uint8_t high4 = letter & (0xF0);
	uint8_t low4  = (letter << 4) & (0xF0);

	/* for write in data register */
	 Write_4bits(high4 | mode );
	 Write_4bits(low4 | mode );

}


void Write_4bits(uint8_t value)
{
	Transmit_byte(value);
	Transmit_byte(value|ENABLE);
	SysTick_Delay(0.02); //20us

	Transmit_byte(value & ~ENABLE);
	SysTick_Delay(0.02); //20us


}


/*D7 D6 D5 D4  BL  E  R/~W  RS */
void Transmit_byte(uint8_t data)
{

  /* Generate START */
  //I2C1->CR1 |= I2C_CR1_START;


  /* Wait SB flag is set */
  while(I2C_SR1_SB != (I2C_SR1_SB & I2C1->SR1))
  {
	  Gpio_on(Red_Led);/* Do nothing */
  }

  Gpio_off(Red_Led);
  /* Read SR1 */
  (void)I2C1->SR1;

  /* Send slave address with write */
  I2C1->DR = (uint8_t) address_board;

  /* Wait ADDR flag is set */
  while(I2C_SR1_ADDR != (I2C_SR1_ADDR & I2C1->SR1))
  {
	 /* Do nothing */
  }


  /* Wait TX flag is set */
   while(I2C_SR1_TXE != (I2C_SR1_TXE & I2C1->SR1))
   {
     /* Do nothing */
   }

   //write the data
   I2C1->DR = (uint8_t) data;

   /* Read SR1 */
   (void)I2C1->SR1;

   /* Read SR2 */
   (void)I2C1->SR2;


   /* Wait BTF flag is set */
     while(I2C_SR1_BTF != (I2C_SR1_BTF & I2C1->SR1))
     {
        /* Do nothing */
     }


     /* Generate STOP */
      I2C1->CR1 |= I2C_CR1_STOP;


}




/**
 * @brief   GPIO Configure, PB8 -> I2C1_SCL, PB9 -> I2C1_SDA
 * @note
 * @param   None
 * @retval  None
 */
void GPIO_Config(void)
{
  /* Enable port B*/
  RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN);

  /* Select Alternate function mode */
  GPIOB->MODER &= ~ (GPIO_MODER_MODER8|GPIO_MODER_MODER9);
  GPIOB->MODER |= (GPIO_MODER_MODER8_1 |GPIO_MODER_MODER9_1);

  /* Select output type open drain */
  GPIOB->OTYPER |= (GPIO_OTYPER_OT_8| GPIO_OTYPER_OT_9);

  /* Select output speed medium speed for [10] as low speed for proteus */
  GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR8 |GPIO_OSPEEDER_OSPEEDR9);
  GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR8_1 |GPIO_OSPEEDER_OSPEEDR9_1);


  /* Select no_pull-up or down */
  GPIOB->PUPDR &= ~ (GPIO_PUPDR_PUPDR8 |GPIO_PUPDR_PUPDR9)  ;
  //GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR8_0 |GPIO_PUPDR_PUPDR9_0);

  /* Select AF4 */
  GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL8 | GPIO_AFRH_AFSEL9);
  GPIOB->AFR[1] |= (GPIO_AFRH_AFSEL8_2 | GPIO_AFRH_AFSEL9_2);

}

/**
 * @brief   I2C1 Master Configuration
 * @note
 * @param   None
 * @retval  None
 */
 void I2C1_Config(void)
{
  /* Enable I2C1 clock */
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

  /* Make sure the peripheral is disabled for clock configuration */
  I2C1->CR1 &= ~I2C_CR1_PE;

  /* Select Peripheral clock frequency, APB1 = 42 MHz  but we make it 20MHz*/
  I2C1->CR2 &= ~I2C_CR2_FREQ;
  I2C1->CR2 |= (I2C_CR2_FREQ_2| I2C_CR2_FREQ_4);

  /* Select slow mode for proteus to generate 100khz */
  I2C1->CCR &= ~I2C_CCR_FS;

  /* Set duty cycle with SM mode */
  //I2C1->CCR &= ~I2C_CCR_DUTY;

  /* CCR Clock configuration for Fast I2C mode
   * T_high =  CCR * T_PCLK1
   * T_low =  CCR * T_PCLK1
   *
   * I2C_Clock_Period = T_high + T_low
   *  = 2*CCR * T_PCLK1
   *
   *  CCR = I2C_Clock_Period / (T_PCLK1 * 2)
   *      = PCLK1_freq / (I2C_Clock_freq * 2)
   *      = 20000000 / (100000 * 2) = 100
   *
   *  because the CCR have (12bits) and then we can use the 100 and check
   * I2C_Clock_freq = PCLK1_freq / (CCR * 2)
   *                 = 20000000 / (2 * 100) = 100000 OK
   *
   *
   * T_high = (100) / 20000000  = 5us OK (in standard should be > 0.6us )
   * T_low  = (100) / 20000000 = 5us OK (in standard should be > 1.3us)
   * Check UM10204 Table 10. */

  I2C1->CCR &= ~I2C_CCR_CCR;
  I2C1->CCR |= (uint16_t) 100u;

  /* Rise time configuration
   * In UM10204 Table 10. max rise time for slow-mode is 1000ns
   * TRISE = (1000ns / 50ns) + 1 = 21
   *
   * if we take 21 then;
   * Rise_Time = (21 - 1) * 50ns = 1000ns (is OK) */

  I2C1->TRISE &= ~I2C_TRISE_TRISE;
  I2C1->TRISE |= (uint8_t) 21u;

  /* Clock stretching enabled */
  I2C1->CR1 &= ~I2C_CR1_NOSTRETCH;

  /* General call disabled */
  I2C1->CR1 &= ~I2C_CR1_ENGC;

  /* Peripheral enable */
  I2C1->CR1 |= I2C_CR1_PE;

  /* Event interrupt enabled */
  I2C1->CR2 |= I2C_CR2_ITEVTEN;

  /* Error interrupt enabled */
  I2C1->CR2 |= I2C_CR2_ITERREN;

  /*Generate START */
    I2C1->CR1 |= I2C_CR1_START;


}



