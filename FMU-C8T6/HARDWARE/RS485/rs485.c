#include "rs485.h"
#include "usart.h"
u8 RS485_RX_BUF[RS485_REC_LEN];
u8 RS485_RX_LEN;
u8 OXYGEN_Flag,PH_Flag,NITRITE_Flag,AMMONIA_Flag,PRESSURE_Flag,LIGHTNESS_Flag;

//USART2,PA2-TX,PA3-RX
//RS485RE,PA4
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
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USART2, ENABLE);  //使能
}

void RS485_SendData(const uint8_t *data, uint8_t len){
  uint8_t i =0;
  RS485_RE(1);
  for(i=0; i<len; i++){
    while((USART2->SR & (1<<6)) == 0);
    USART2->DR = data[i];
  }
  while((USART2->SR & (1<<6)) == 0);
  RS485_RE(0);
}

void USART2_IRQHandler(void){
  if(USART2->SR&(1<<5)){
    RS485_RX_BUF[RS485_RX_LEN++]=USART2->DR;
  }
  if(USART2->SR&1<<4){
    RS485_RX_BUF[RS485_RX_LEN]=0;  //字符串结束标志
		//printf("!!! %s\r\n", RS485_RX_BUF);
		USART2->SR;
		USART2->DR;
	}
}

void RS485_ReadData(uint8_t *data, uint8_t *len){
	uint8_t i;
	*len = RS485_RX_LEN;
	for(i=0;i<RS485_RX_LEN;i++){
		data[i] = RS485_RX_BUF[i];
	}
	RS485_RX_LEN = 0;
	RS485_RX_BUF[RS485_RX_LEN] = 0;
	
	if(RS485_RX_BUF[0]==0x10){
		OXYGEN_Flag = 1;
	}else if(RS485_RX_BUF[0]==0x11){
		PH_Flag = 1;
	}else if(RS485_RX_BUF[0]==0x12){
		NITRITE_Flag = 1;
	}else if(RS485_RX_BUF[0]==0x13){
		AMMONIA_Flag = 1;
	}else if(RS485_RX_BUF[0]==0x01){
		PRESSURE_Flag = 1;
	}else if(RS485_RX_BUF[0]==0x02){
		LIGHTNESS_Flag = 1;
	}
}
