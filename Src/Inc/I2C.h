#ifndef INC_I2C_H_
#define INC_I2C_H_


/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "stm32f4xx.h"





void GPIO_Config(void);
void I2C1_Config(void);
void Transmit_byte(uint8_t data);
void Write_4bits(uint8_t value);
void send(uint8_t letter , uint8_t mode);
void send_ch(uint8_t letter);
void send_cmd(uint8_t cmd);
void send_string(char *str );
void set_cursor(uint8_t row, uint8_t col);
void Lcd_Init(void);



/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif /* INC_I2C_H_ */
