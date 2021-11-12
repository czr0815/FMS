#include "sensor.h"
#include "rs485.h"
#include "crc.h"

void Sensor_Init(void){
	const uint8_t oxygen_init[8]={0x10, 0x06, 0x11, 0x00, 0x00, 0x01, 0x4E, 0x77};
	RS485_SendData(oxygen_init, 8);
}

void getOxygenData(void){
	const uint8_t data[8]={0x10, 0x03, 0x00, 0x00, 0x00, 0x04, 0x47, 0x48};
	RS485_SendData(data, 8);
}

void getPhData(void){
	const uint8_t data[8]={0x11, 0x03, 0x00, 0x00, 0x00, 0x04, 0x46, 0x99};
	RS485_SendData(data, 8);
}

void getNitriteData(void){
	const uint8_t data[8]={0x12, 0x03, 0x00, 0x01, 0x00, 0x02, 0x97, 0x68,};
	RS485_SendData(data, 8);
}

void getAmmoniaData(void){
	const uint8_t data[8]={0x13, 0x03, 0x00, 0x00, 0x00, 0x04, 0x47, 0x7B};
	RS485_SendData(data, 8);
}

void getPressureData(void){
	const uint8_t data[8]={0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B};
	RS485_SendData(data, 8);
}

void getLightnessData(void){
	const uint8_t data[8]={0x02, 0x03, 0x00, 0x02, 0x00, 0x02, 0x65, 0xF8};
	RS485_SendData(data, 8);
}

void parseOxygenData(uint8_t *data, double *oxygen, double *temperature){
	uint16_t i;
	uint16_t raw_oxygen=((data[3]<<8)|data[4]);
	uint16_t point_oxygen=((data[5]<<8)|data[6]);
	uint16_t raw_temperature=((data[7]<<8)|data[8]);
	uint16_t point_temperature=((data[9]<<8)|data[10]);
	*oxygen=raw_oxygen*1.0;
	for(i=0;i<point_oxygen;i++){
		*oxygen=*oxygen*0.1;
	}
	*temperature=raw_temperature*1.0;
	for(i=0;i<point_temperature;i++){
		*temperature=*temperature*0.1;
	}
}

void parsePhData(uint8_t *data, double *ph){
	uint16_t i;
	uint16_t raw=((data[3]<<8)|data[4]);
	uint16_t point=((data[5]<<8)|data[6]);
	*ph=raw*1.0;
	for(i=0;i<point;i++){
		*ph=*ph*0.1;
	}
}

void parseNitriteData(uint8_t *data, double *nitrite){
	uint16_t i;
	uint16_t raw=((data[3]<<8)|data[4]);
	uint16_t point=((data[5]<<8)|data[6]);
	*nitrite=raw*1.0;
	for(i=0;i<point;i++){
		*nitrite=*nitrite*0.1;
	}
}

void parseAmmoniaData(uint8_t *data, double *ammonia){
	uint16_t i;
	uint16_t raw=((data[3]<<8)|data[4]);
	uint16_t point=((data[5]<<8)|data[6]);
	*ammonia=raw*1.0;
	for(i=0;i<point;i++){
		*ammonia=*ammonia*0.1;
	}
}

void parsePressureData(uint8_t *data, double *pressure){
	uint16_t raw=((data[3]<<8)|data[4]);
	*pressure=raw*1.0;
	*pressure=*pressure*0.1;
}

void parseLightnessData(uint8_t *data, uint32_t *lightness){
	*lightness=((data[3]<<24)|(data[4]<<16)|(data[5]<<8)|data[6]);
}

void uint64To8(uint64_t a, uint8_t *b){
	b[0]=(uint8_t)(a>>56);
	b[1]=(uint8_t)(a>>48);
	b[2]=(uint8_t)(a>>40);
	b[3]=(uint8_t)(a>>32);
	b[4]=(uint8_t)(a>>24);
	b[5]=(uint8_t)(a>>16);
	b[6]=(uint8_t)(a>>8);
	b[7]=(uint8_t)a;
}
void uint32To8(uint32_t a, uint8_t *b){
	b[0]=(uint8_t)(a>>24);
	b[1]=(uint8_t)(a>>16);
	b[2]=(uint8_t)(a>>8);
	b[3]=(uint8_t)a;
}

void uint16To8(uint16_t a, uint8_t *b){
	b[0]=(uint8_t)(a>>8);
	b[1]=(uint8_t)a;
}
