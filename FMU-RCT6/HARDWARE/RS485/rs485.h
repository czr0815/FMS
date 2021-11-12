#include "stm32f10x.h"

#define RS485_REC_LEN 32

void RS485_Init(uint32_t bound);
void RS485_SendData(const uint8_t *data, uint8_t len);
void RS485_ReadData(uint8_t *data, uint8_t *len);

//extern u8 OXYGEN_Flag,PH_Flag,NITRITE_Flag,AMMONIA_Flag,PRESSURE_Flag,LIGHTNESS_Flag;

void Oxygen_ReadData(uint8_t *data, uint8_t *len);
void Ph_ReadData(uint8_t *data, uint8_t *len);
void Nitrite_ReadData(uint8_t *data, uint8_t *len);
void Ammonia_ReadData(uint8_t *data, uint8_t *len);
void Pressure_ReadData(uint8_t *data, uint8_t *len);
void Lightness_ReadData(uint8_t *data, uint8_t *len);

