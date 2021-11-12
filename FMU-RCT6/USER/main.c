#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "rs485.h"
#include "sensor.h"
#include "common.h"
#include "http.h"
#include "FreeRTOS.h"
#include "task.h"

//MCU的ID获取地址
#ifdef __STM32F00x_H
#define  ID_ADDR1  0x1FFFF7AC      /*STM32F0唯一ID起始地址*/
#endif
#ifdef __STM32F10x_H
#define  ID_ADDR1  0x1FFFF7E8     /*STM32F1唯一ID起始地址*/
#endif
#ifdef __STM32F2xx_H
#define  ID_ADDR1  0x1FFF7A10         /*STM32F2唯一ID起始地址*/
#endif
#ifdef __STM32F3xx_H
#define  ID_ADDR1  0x1FFFF7AC        /*STM32F3唯一ID起始地址*/
#endif
#ifdef __STM32F4xx_H
#define  ID_ADDR1  0x1FFF7A10        /*STM32F4唯一ID起始地址*/
#endif
#ifdef __STM32F7xx_H
#define  ID_ADDR1  0x1FF0F420        /*STM32F7唯一ID起始地址*/
#endif
#ifdef  __STM32L0XX_H
#define  ID_ADDR1  0x1FF80050        /*STM32L0唯一ID起始地址*/
#endif
#ifdef __STM32L1XX_H
#define  ID_ADDR1  0x1FF80050        /*STM32L1唯一ID起始地址*/
#endif
#ifdef __STM32L4xx_H
#define  ID_ADDR1  0x1FFF7590        /*STM32L4唯一ID起始地址*/
#endif
#ifdef __STM32H7xx_H
#define  ID_ADDR1  0x1FF0F420        /*STM32H7唯一ID起始地址*/
#endif


#define START_TASK_PRIO 0
#define START_TASK_SIZE 128
TaskHandle_t StartTaskHandler;
void start_task(void* param);

#define LED2_TASK_PRIO 10
#define LED2_TASK_SIZE 64
TaskHandle_t Led2TaskHandler;
void led2_task(void *param);

#define LED3_TASK_PRIO 9
#define LED3_TASK_SIZE 64
TaskHandle_t Led3TaskHandler;
void led3_task(void *param);

#define LED3_REC_TASK_PRIO 9
#define LED3_REC_TASK_SIZE 64
TaskFunction_t Led3RecTaskHandler;
void led3_rec_task(void *param);

#define KEY_TASK_PRIO 8
#define KEY_TASK_SIZE 64
TaskHandle_t KeyTaskHandler;
void key_task(void *param);

#define USART3_TASK_PRIO 7
#define USART3_TASK_SIZE 64
TaskHandle_t Usart3TaskHandler;
void usart3_task(void *param);

#define DATA_TASK_PRIO 6
#define DATA_TASK_SIZE 512
TaskHandle_t DataTaskHandler;
void data_task(void* param);

#define OXYGEN_TASK_PRIO 6
#define OXYGEN_TASK_SIZE 256
TaskHandle_t OxygenTaskHandler;
void oxygen_task(void* param);

#define PH_TASK_PRIO 5
#define PH_TASK_SIZE 256
TaskHandle_t PhTaskHandler;
void ph_task(void* param);

#define NITRITE_TASK_PRIO 4
#define NITRITE_TASK_SIZE 256
TaskHandle_t NitriteTaskHandler;
void nitrite_task(void* param);

#define AMMONIA_TASK_PRIO 3
#define AMMONIA_TASK_SIZE 256
TaskHandle_t AmmoniaTaskHandler;
void ammonia_task(void* param);

#define PRESSURE_TASK_PRIO 2
#define PRESSURE_TASK_SIZE 256
TaskHandle_t PressureTaskHandler;
void pressure_task(void* param);

#define LIGHTNESS_TASK_PRIO 1
#define LIGHTNESS_TASK_SIZE 256
TaskHandle_t LightnessTaskHandler;
void lightness_task(void* param);

void delay_s(uint8_t n);
void upload_data(char *id, char *type, double value);
void upload_online(char *id,uint8_t status);
void get_ID(void);
char *http_char_data = NULL;
char *http_char_online = NULL;
	
const static char *SERVER_IP = "1.117.233.138";
const static char *SERVER_PORT = "8888";
char ID[40];
u8 esp_flag, led3_flag;

int main(void){
	delay_init();
	uart_init(9600);
	usart3_init(115200);
	RS485_Init(9600);
	KEY_Init();
	LED_Init();
	LED1 = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	get_ID();
	printf("Device ID:%s\r\n",ID);
	printf("FMU开始初始化...\r\n");
	
	xTaskCreate((TaskFunction_t)start_task,         //任务函数
              (const char*)"start_task",          //任务名称
              (uint16_t)START_TASK_SIZE,          //任务堆栈大小
              (void*)NULL,                        //任务优先级
              (UBaseType_t)START_TASK_PRIO,       //任务句柄
              (TaskHandle_t*)&StartTaskHandler);			
	vTaskStartScheduler();
}


void start_task(void *pvParameters){
	taskENTER_CRITICAL();
	//Sensor_Init();

	esp_flag = led3_flag = 0;
	xTaskCreate((TaskFunction_t   )led2_task,
	            (const char*      )"led2_task",
							(uint16_t         )LED2_TASK_SIZE,
							(void*            )NULL,
							(UBaseType_t      )LED2_TASK_PRIO,
							(TaskHandle_t*    )&Led2TaskHandler);
	if(!esp8266_init_sta()){
		//没有连接到目标wifi
		printf("没有连接到网络!\r\n");
	}else{
		LED4 = 1;
		printf("已连接到目标wifi\r\n");
	}
	upload_online(ID, 0);	
							
		
	//创建监听Key任务
  xTaskCreate((TaskFunction_t   )key_task,
              (const char*      )"key_task",
              (uint16_t         )KEY_TASK_SIZE,
              (void*            )NULL,
              (UBaseType_t      )KEY_TASK_PRIO,
              (TaskHandle_t*    )&KeyTaskHandler);
	//创建监听Usart3任务
  xTaskCreate((TaskFunction_t   )usart3_task,
              (const char*      )"usart3_task",
              (uint16_t         )USART3_TASK_SIZE,
              (void*            )NULL,
              (UBaseType_t      )USART3_TASK_PRIO,
              (TaskHandle_t*    )&Usart3TaskHandler);
	//创建数据采集任务
  xTaskCreate((TaskFunction_t   )data_task,
              (const char*      )"data_task",
              (uint16_t         )DATA_TASK_SIZE,
              (void*            )NULL,
              (UBaseType_t      )DATA_TASK_PRIO,
              (TaskHandle_t*    )&DataTaskHandler);
	/*
	//创建溶氧+温度任务
  xTaskCreate((TaskFunction_t   )oxygen_task,
              (const char*      )"oxygen_task",
              (uint16_t         )OXYGEN_TASK_SIZE,
              (void*            )NULL,
              (UBaseType_t      )OXYGEN_TASK_PRIO,
              (TaskHandle_t*    )&OxygenTaskHandler);
  //创建PH任务
  xTaskCreate((TaskFunction_t   )ph_task,
              (const char*      )"ph_task",
              (uint16_t         )PH_TASK_SIZE,
              (void*            )NULL,
              (UBaseType_t      )PH_TASK_PRIO,
              (TaskHandle_t*    )&PhTaskHandler);
	//创建亚硝酸盐任务
	xTaskCreate((TaskFunction_t   )nitrite_task,
							(const char*      )"nitrite_task",
							(uint16_t         )NITRITE_TASK_SIZE,
							(void*            )NULL,
							(UBaseType_t      )NITRITE_TASK_PRIO,
							(TaskHandle_t*    )&NitriteTaskHandler);
	//创建氨氮任务
	xTaskCreate((TaskFunction_t   )ammonia_task,
							(const char*      )"ammonia_task",
							(uint16_t         )AMMONIA_TASK_SIZE,
							(void*            )NULL,
							(UBaseType_t      )AMMONIA_TASK_PRIO,
							(TaskHandle_t*    )&AmmoniaTaskHandler);
  //创建大气压任务
  xTaskCreate((TaskFunction_t   )pressure_task,
              (const char*      )"pressure_task",
              (uint16_t         )PRESSURE_TASK_SIZE,
              (void*            )NULL,
              (UBaseType_t      )PRESSURE_TASK_PRIO,
              (TaskHandle_t*    )&PressureTaskHandler);
  //创建光照任务
  xTaskCreate((TaskFunction_t   )lightness_task,
              (const char*      )"lightness_task",
              (uint16_t         )LIGHTNESS_TASK_SIZE,
              (void*            )NULL,
              (UBaseType_t      )LIGHTNESS_TASK_PRIO,
              (TaskHandle_t*    )&LightnessTaskHandler);
	*/
							
	// 删除开始任务
  vTaskDelete(StartTaskHandler);
	printf("FMU初始化完毕\r\n");
	taskEXIT_CRITICAL();
}

void led2_task(void* param){
	while(1){
		if(led3_flag == 1){
			xTaskCreate((TaskFunction_t   )led3_task,
	            (const char*      )"led3_task",
							(uint16_t         )LED3_TASK_SIZE,
							(void*            )NULL,
							(UBaseType_t      )LED3_TASK_PRIO,
							(TaskHandle_t*    )&Led3TaskHandler);
		}
		LED2 = 0;
		vTaskDelay(1000);
		LED2 = 1;
		vTaskDelay(1000);
	}
}

void led3_task(void* param){
	while(1){
		LED3 = 0;
		vTaskDelay(1000);
		LED3 = 1;
		vTaskDelay(1000);
	}
}

void led3_rec_task(void *param){
	u8 i = 3;
	while(i--){
		LED3 = 0;
		vTaskDelay(150);
		LED3 = 1;
		vTaskDelay(150);
	}
	vTaskDelete(NULL);
}

void key_task(void* param){
	while(1){
		uint8_t key=KEY_Scan(0);
	  if(key==KEY0_PRES&&esp_flag!=0){
		  //进入工作模式
			taskENTER_CRITICAL();
			esp_flag = led3_flag = 0;
			LED3 = LED4 = 0;
			xTaskCreate((TaskFunction_t   )led2_task,
	            (const char*      )"led2_task",
							(uint16_t         )LED2_TASK_SIZE,
							(void*            )NULL,
							(UBaseType_t      )LED2_TASK_PRIO,
							(TaskHandle_t*    )&Led2TaskHandler);
			taskEXIT_CRITICAL();
			if(!esp8266_init_sta()){
				printf("没有连接到目标wifi!\r\n");
			}else{
				LED4 = 1;
				printf("已连接到目标wifi\r\n");
			}
			upload_online(ID, 0);	
	  }else if(key==KEY1_PRES&&esp_flag!=1){
		  //进入配置模式
			taskENTER_CRITICAL();
		  esp_flag = 1;
			if(led3_flag == 1){
				vTaskDelete(Led3TaskHandler);
			}
			LED4 = LED3 = 0;
			vTaskDelete(Led2TaskHandler);
			LED2 = 1;
			taskEXIT_CRITICAL();
		  if(!esp8266_init_ap()){
				printf("进入网络配置模式失败\r\n");
			}else{
				printf("进入网络配置模式\r\n");
				LED3 = 1;
			}
	  }
		vTaskDelay(300);
	}
}

void usart3_task(void *param){
	while(1){
		//taskENTER_CRITICAL();
		if(USART3_RX_STA&0X8000){
		  uint8_t rlen=USART3_RX_STA&0X7FFF;	//得到本次接收到的数据长度
      USART3_RX_BUF[rlen]=0;		//添加结束符
      //sprintf((char*)p,"收到%d字节,内容如下",rlen);//接收到的字节数
      //printf("%s\r\n",p); 			//显示接收到的数据长度
      //printf("接收数据:%s\r\n",USART3_RX_BUF);//显示接收到的数据
      USART3_RX_STA=0;
		  // USART3收到的数据进行解析
			if(esp_flag){
				xTaskCreate((TaskFunction_t   )led3_rec_task,
	                  (const char*      )"led3_rec_task",
							      (uint16_t         )LED3_REC_TASK_SIZE,
							      (void*            )NULL,
							      (UBaseType_t      )LED3_REC_TASK_PRIO,
							      (TaskHandle_t*    )&Led3RecTaskHandler);
			}
		  recv_data_analysis(esp_flag, USART3_RX_BUF);
	  }
		//taskEXIT_CRITICAL();
		vTaskDelay(800);
	}
}

void data_task(void* param) {
	uint8_t oxygen_data[13], ph_data[13], nitrite_data[13], ammonia_data[13], pressure_data[13], lightness_data[13], len;
  double oxygen, temperature, ph, nitrite, ammonia, pressure;
	uint32_t lightness;
	while(1){
		LED1 = 1;
		
		printf("正在获取PH值...\r\n");
		getPhData();
		vTaskDelay(3000);
		RS485_ReadData(ph_data, &len);
		//Ph_ReadData(ph_data, &len);
		if(len != 0){
			parsePhData(ph_data, &ph);
			printf("PH值:%.2f\r\n", ph);
			upload_data(ID, "ph", ph);
		}else{
			printf("PH数据未获得！\r\n");
		}
		 
		printf("正在获取溶氧量和温度数据...\r\n");
		getOxygenData();
		vTaskDelay(3000);
		RS485_ReadData(oxygen_data, &len);
		//Oxygen_ReadData(oxygen_data, &len);
		if(len != 0){
			parseOxygenData(oxygen_data, &oxygen, &temperature);
		  printf("溶氧量:%.2fmg/L, 温度:%.1f℃\r\n", oxygen, temperature);
			upload_data(ID, "oxygen",oxygen);
			vTaskDelay(200);
			upload_data(ID, "temperature", temperature);
		}else{
			printf("溶氧量和温度数据未获得！\r\n");
		}
		
		
		printf("正在获取氨氮浓度...\r\n");
		getAmmoniaData();
		vTaskDelay(3000);
		RS485_ReadData(ammonia_data, &len);
		//Ammonia_ReadData(ammonia_data, &len);
		if(len != 0){
			parseAmmoniaData(ammonia_data, &ammonia);
			printf("氨氮浓度:%.2f\r\n", ammonia);
			upload_data(ID, "ammoniaNitrogen", ammonia);
		}else{
			printf("氨氮浓度未获得！\r\n");
		}
		
		
		printf("正在获取亚硝酸浓度...\r\n");
		getNitriteData();
		vTaskDelay(3000);
		RS485_ReadData(nitrite_data, &len);
		if(len != 0){
			parseNitriteData(nitrite_data, &nitrite);
			printf("亚硝酸盐浓度:%.2f\r\n", nitrite);
			upload_data(ID, "nitrite", nitrite);
		}else{
			printf("亚硝酸盐浓度未获得！\r\n");
		}
		
		
		printf("正在获取大气压数据...\r\n");
		getPressureData();
		vTaskDelay(3000);
		RS485_ReadData(pressure_data, &len);
		//Pressure_ReadData(pressure_data, &len);
		if(len != 0){
			parsePressureData(pressure_data, &pressure);
			printf("大气压:%.2fkPa\r\n", pressure);
			upload_data(ID, "pressure", pressure);
		}else{
			printf("大气压数据未获得！\r\n");
		}
		
		printf("正在获取光照数据...\r\n");
		getLightnessData();
		vTaskDelay(3000);
		RS485_ReadData(lightness_data, &len);
		//Lightness_ReadData(lightness_data, &len);
		if(len != 0){
			parseLightnessData(lightness_data, &lightness);
			printf("光照强度:%dLux\r\n", lightness);
			upload_data(ID, "lightness", lightness);
		}else{
			printf("光照强度数据未获得！\r\n");
		}
		
		LED1 = 0;
		vTaskDelay(30000);
  }
}

void oxygen_task(void* param){
  uint8_t data[13],len;
  double oxygen, temperature;
  while(1){
		LED1 = 1;
		printf("正在获取溶氧量和温度数据...\r\n");
		taskENTER_CRITICAL();
		getOxygenData();
		delay_ms(800);
		RS485_ReadData(data, &len);
		taskEXIT_CRITICAL();
		if(len != 0){
			parseOxygenData(data, &oxygen, &temperature);
		  printf("溶氧量:%.2fmg/L, 温度:%.1f℃\r\n", oxygen, temperature);
			upload_data(ID, "oxygen",oxygen);
			delay_ms(200);
			upload_data(ID, "temperature", temperature);
		}else{
			printf("溶氧量和温度数据未获得！\r\n");
		}
		LED1 = 0;
		vTaskDelay(30000);
  }
}

void ph_task(void* param){
  uint8_t data[13],len;
  double ph;
  while(1){
		LED1 = 1;
		printf("正在获取PH值...\r\n");
		taskENTER_CRITICAL();
		getPhData();
		delay_ms(800);
		RS485_ReadData(data, &len);
		taskEXIT_CRITICAL();
		if(len != 0){
			parsePhData(data, &ph);
			printf("PH值:%.2f\r\n", ph);
			upload_data(ID, "ph", ph);
		}else{
			printf("PH数据未获得！\r\n");
		}
		LED1 = 0;
		vTaskDelay(30000);
  }
}

void nitrite_task(void* param){
  uint8_t data[13],len;
  double nitrite;
  while(1){
		LED1 = 1;
		printf("正在获取亚硝酸浓度...\r\n");
		taskENTER_CRITICAL();
		getNitriteData();
		delay_ms(800);
		RS485_ReadData(data, &len);
		taskEXIT_CRITICAL();
		if(len != 0){
			parseNitriteData(data, &nitrite);
			printf("亚硝酸盐浓度:%.2f\r\n", nitrite);
			upload_data(ID, "nitrite", nitrite);
		}else{
			printf("亚硝酸盐浓度未获得！\r\n");
		}
		LED1 = 0;
		vTaskDelay(30000);
  }
}

void ammonia_task(void* param){
  uint8_t data[13],len;
  double ammonia;
  while(1){
		LED1 = 1;
		printf("正在获取氨氮浓度...\r\n");
		taskENTER_CRITICAL();
		getAmmoniaData();
		delay_ms(800);
		RS485_ReadData(data, &len);
		taskEXIT_CRITICAL();
		if(len != 0){
			parseAmmoniaData(data, &ammonia);
			printf("氨氮浓度:%.2f\r\n", ammonia);
			upload_data(ID, "ammoniaNitrogen", ammonia);
		}else{
			printf("氨氮浓度未获得！\r\n");
		}
		LED1 = 0;
		vTaskDelay(30000);
  }
}

void pressure_task(void* param){
  uint8_t data[8],len;
  double pressure;
  while(1){
		LED1 = 1;
		printf("正在获取大气压数据...\r\n");
		taskENTER_CRITICAL();
		getPressureData();
		delay_ms(800);
		RS485_ReadData(data, &len);
		taskEXIT_CRITICAL();
		if(len != 0){
			parsePressureData(data, &pressure);
			printf("大气压:%.2fkPa\r\n", pressure);
			upload_data(ID, "pressure", pressure);
		}else{
			printf("大气压数据未获得！\r\n");
		}
		LED1 = 0;
    vTaskDelay(30000);
  }
}

void lightness_task(void* param){
  uint8_t data[9],len;
  uint32_t lightness;
  while(1){
		LED1 = 1;
		printf("正在获取光照数据...\r\n");
		taskENTER_CRITICAL();
		getLightnessData();
		delay_ms(800);
		RS485_ReadData(data, &len);
		taskEXIT_CRITICAL();
		if(len != 0){
			parseLightnessData(data, &lightness);
			printf("光照强度:%dLux\r\n", lightness);
			upload_data(ID, "lightness", lightness);
		}else{
			printf("光照强度数据未获得！\r\n");
		}
		LED1 = 0;
		vTaskDelay(30000);
  }
}


void delay_s(uint8_t n){
	uint8_t i;
	for(i=0;i<n;i++){
		delay_ms(1000);
	}
}

void get_ID(){
	uint32_t temp0, temp1, temp2;
	uint8_t temp[12];
	temp0=*(__IO uint32_t*)(ID_ADDR1);
	temp1=*(__IO uint32_t*)(ID_ADDR1+4);
	temp2=*(__IO uint32_t*)(ID_ADDR1+8);
	temp[0]=(temp0>>24)&0xFF;
	temp[1]=(temp0>>16)&0xFF;
	temp[2]=(temp0>>8)&0xFF;
	temp[3]=temp0&0xFF;
	temp[4]=(temp1>>24)&0xFF;
	temp[5]=(temp1>>16)&0xFF;
	temp[6]=(temp1>>8)&0xFF;
	temp[7]=temp1&0xFF;
	temp[8]=(temp2>>24)&0xFF;
	temp[9]=(temp2>>16)&0xFF;
	temp[10]=(temp2>>8)&0xFF;
	temp[11]=temp2&0xFF;
	memset(ID,0,40);
	sprintf(ID,"%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X",temp[0],temp[1],temp[2],temp[3],temp[4],temp[5],temp[6],temp[7],temp[8],temp[9],temp[10],temp[11]);
}

void upload_data(char *id, char *type, double value){
	http_char_data = mymalloc(SRAMIN, TOTAL_DATA_LEN);
	if(NULL == http_char_data) return ;
	if(esp_flag) return ;
	memset(http_char_data,0,strlen(http_char_data));
	atk_8266_quit_trans();
  atk_8266_send_cmd("AT+CIPSEND","OK",20);
	Http_Data_Upload(SERVER_IP, SERVER_PORT, id, type, value, http_char_data);
	//printf("%s",http_char_data);
	u3_printf("%s",http_char_data);
	myfree(SRAMIN, http_char_data);
}

void upload_online(char *id, uint8_t status){
	http_char_online = mymalloc(SRAMIN, TOTAL_DATA_LEN);
	if(NULL == http_char_online) return ;
	if(esp_flag) return ;
	memset(http_char_data,0,strlen(http_char_online));
	atk_8266_quit_trans();
  atk_8266_send_cmd("AT+CIPSEND","OK",20);
	Http_Online_Upload(SERVER_IP, SERVER_PORT, id, status, http_char_online);
	//printf("%s",http_char_online);
	u3_printf("%s",http_char_online);
	myfree(SRAMIN, http_char_online);
}
