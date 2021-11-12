#include "stm32f10x.h"

//设为1为发送模式，设为0为接收模式
#define RS485_RE(x) GPIO_SetBits(GPIOA,GPIO_Pin_4)

#define RS485_REC_LEN 32

void RS485_Init(uint32_t bound);
void RS485_SendData(const uint8_t *data, uint8_t len);
void RS485_ReadData(uint8_t *data, uint8_t *len);

extern u8 OXYGEN_Flag,PH_Flag,NITRITE_Flag,AMMONIA_Flag,PRESSURE_Flag,LIGHTNESS_Flag;
/*
void Oxygen_ReadData(uint8_t *data, uint8_t *len);
void Ph_ReadData(uint8_t *data, uint8_t *len);
void Pressure_ReadData(uint8_t *data, uint8_t *len);
void Lightness_ReadData(uint8_t *data, uint8_t *len);
*/
