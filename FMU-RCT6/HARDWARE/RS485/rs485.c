#include "rs485.h"
#include "usart.h"
u8 RS485_RX_BUF[RS485_REC_LEN];
u8 RS485_RX_LEN;
u8 OXYGEN_LEN, PH_LEN, NITRITE_LEN, AMMONIA_LEN, PRESSURE_LEN, LIGHTNESS_LEN;
u8 OXYGEN_BUF[RS485_REC_LEN], PH_BUF[RS485_REC_LEN], NITRITE_BUF[RS485_REC_LEN], AMMONIA_BUF[RS485_REC_LEN], PRESSURE_BUF[RS485_REC_LEN], LIGHTNESS_BUF[RS485_REC_LEN];
//u8 OXYGEN_Flag,PH_Flag,NITRITE_Flag,AMMONIA_Flag,PRESSURE_Flag,LIGHTNESS_Flag;

//USART2,PA2-TX,PA3-RX
//RS485RE,PA8
void RS485_Init(uint32_t bound){
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

	//GPIO
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//USART2
	USART_InitStructure.USART_BaudRate = bound;//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART2, &USART_InitStructure);

	//Usart2 NVIC 
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	

  RS485_RX_LEN = 0;
	OXYGEN_LEN = PH_LEN = AMMONIA_LEN = NITRITE_LEN = PRESSURE_LEN = LIGHTNESS_LEN = 0;
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USART2, ENABLE);  //使能
}

void RS485_SendData(const uint8_t *data, uint8_t len){
  uint8_t i =0;
  //进入发送模式
	GPIO_ResetBits(GPIOA,GPIO_Pin_11);
  for(i=0; i<len; i++){
    while((USART2->SR & (1<<6)) == 0);
    USART2->DR = data[i];
  }
  while((USART2->SR & (1<<6)) == 0);
  //进入接收模式
	GPIO_SetBits(GPIOA,GPIO_Pin_11);
}

void USART2_IRQHandler(void){
	uint8_t i;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE)!=RESET){
		RS485_RX_BUF[RS485_RX_LEN++]=USART_ReceiveData(USART2);
	}
	
	if(USART_GetITStatus(USART2, USART_IT_IDLE)!=RESET){
		USART2->DR;
		USART2->SR;
	}
	
	/*
	if(USART2->SR&(1<<5)){
    RS485_RX_BUF[RS485_RX_LEN++]=USART2->DR;
		if(RS485_RX_BUF[0]==0x10&&RS485_RX_LEN==13){
			OXYGEN_LEN = 13;
      for(i=0;i<RS485_RX_LEN;i++){
				OXYGEN_BUF[i] = RS485_RX_BUF[i];
				RS485_RX_BUF[i] = 0;
				printf("%d %d\r\n", i, OXYGEN_BUF[i]);
			}
      RS485_RX_LEN = 0;			
		} else if(RS485_RX_BUF[0]==0x11&&RS485_RX_LEN==13){
			PH_LEN = 13;
			for(i=0;i<RS485_RX_LEN;i++){
				PH_BUF[i] = RS485_RX_BUF[i];
				RS485_RX_BUF[i] = 0;
				printf("%d %d\r\n", i, PH_BUF[i]);
			}
			RS485_RX_LEN = 0;
		} else if(RS485_RX_BUF[0]==0x12&&RS485_RX_LEN==9){
			NITRITE_LEN = 9;
			for(i=0;i<RS485_RX_LEN;i++){
				NITRITE_BUF[i] = RS485_RX_BUF[i];
				RS485_RX_BUF[i] = 0;
				printf("%d %d\r\n", i ,NITRITE_BUF[i]);
			}
			RS485_RX_LEN = 0;
		} else if(RS485_RX_BUF[0]==0x13&&RS485_RX_LEN==13){
			AMMONIA_LEN = 13;
			for(i=0;i<RS485_RX_LEN;i++){
				AMMONIA_BUF[i] = RS485_RX_BUF[i];
				RS485_RX_BUF[i] = 0;
				printf("%d %d\r\n", i ,AMMONIA_BUF[i]);
			}
			RS485_RX_LEN = 0;
		} else if(RS485_RX_BUF[0]==0x01&&RS485_RX_LEN==9){
			PRESSURE_LEN = 9;
			for(i=0;i<RS485_RX_LEN;i++){
				PRESSURE_BUF[i] = RS485_RX_BUF[i];
				RS485_RX_BUF[i] = 0;
				printf("%d %d\r\n",i, PRESSURE_BUF[i]);
			}
			RS485_RX_LEN = 0;
		} else if(RS485_RX_BUF[0]==0x02&&RS485_RX_LEN==9){
			LIGHTNESS_LEN = 9;
			for(i=0;i<RS485_RX_LEN;i++){
				LIGHTNESS_BUF[i] = RS485_RX_BUF[i];
				RS485_RX_BUF[i] = 0;
				printf("%d %d\r\n",i, LIGHTNESS_BUF[i]);
			}
			RS485_RX_LEN = 0;
		}
  }
	*/
	
	
	/*
	if(USART_GetITStatus(USART2, USART_IT_IDLE)!=RESET){
	*/
	
	/*
  if(USART2->SR&1<<4){
		printf("end %d\r\n", RS485_RX_LEN);
		RS485_RX_BUF[RS485_RX_LEN] = 0;
		USART2->SR;
		USART2->DR;
	}
	*/
}

void RS485_ReadData(uint8_t *data, uint8_t *len){
	uint8_t i;
	*len = RS485_RX_LEN;
	for(i=0;i<RS485_RX_LEN;i++){
		data[i] = RS485_RX_BUF[i];
		printf("%d %d\r\n", i, data[i]);
	}
	RS485_RX_LEN = 0;
	RS485_RX_BUF[RS485_RX_LEN] = 0;
}

void Oxygen_ReadData(uint8_t *data, uint8_t *len){
	uint8_t i;
	*len = OXYGEN_LEN;
	for(i=0;i<OXYGEN_LEN;i++){
		data[i] = OXYGEN_BUF[i];
	}
	OXYGEN_LEN = 0;
}

void Ph_ReadData(uint8_t *data, uint8_t *len){
	uint8_t i;
	*len = PH_LEN;
	for(i=0;i<PH_LEN;i++){
		data[i] = PH_BUF[i];
	}
	PH_LEN = 0;
}

void Nitrite_ReadData(uint8_t *data, uint8_t *len){
	uint8_t i;
	*len = NITRITE_LEN;
	for(i=0;i<NITRITE_LEN;i++){
		data[i] = NITRITE_BUF[i];
	}
	NITRITE_LEN = 0;
}

void Ammonia_ReadData(uint8_t *data, uint8_t *len){
	uint8_t i;
	*len = AMMONIA_LEN;
	for(i=0;i<AMMONIA_LEN;i++){
		data[i] = AMMONIA_BUF[i];
	}
	AMMONIA_LEN = 0;
}

void Pressure_ReadData(uint8_t *data, uint8_t *len){
	uint8_t i;
	*len = PRESSURE_LEN;
	for(i=0;i<PRESSURE_LEN;i++){
		data[i] = PRESSURE_BUF[i];
	}
	PRESSURE_LEN = 0;
}

void Lightness_ReadData(uint8_t *data, uint8_t *len){
	uint8_t i;
	*len = LIGHTNESS_LEN;
	for(i=0;i<LIGHTNESS_LEN;i++){
		data[i] = LIGHTNESS_BUF[i];
	}
	LIGHTNESS_LEN = 0;
}

