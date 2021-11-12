#include "stm32f10x.h"

void Sensor_Init(void);

/**
 ��������������ַ10
 PH����������ַ11
 �������δ���������ַ12
 ��������������ַ13
 ˮ���������ַ14
 ����ѹ����������ַ01
 ���մ���������ַ02
**/
void getOxygenData(void);
void getPhData(void);
void getNitriteData(void);
void getAmmoniaData(void);
void getDepthData(void);
void getPressureData(void);
void getLightnessData(void);

void parseOxygenData(uint8_t *data, double *oxygen, double *temperature);
void parsePhData(uint8_t *data, double *ph);
void parseNitriteData(uint8_t *data, double *nitrite);
void parseAmmoniaData(uint8_t *data, double *ammonia);
void parseDepthData(uint8_t *data, double *depth);
void parsePressureData(uint8_t *data, double *pressure);
void parseLightnessData(uint8_t *data, uint32_t *lightness);

void uint64To8(uint64_t a, uint8_t *b);
void uint32To8(uint32_t a, uint8_t *b);
void uint16To8(uint16_t a, uint8_t *b);
