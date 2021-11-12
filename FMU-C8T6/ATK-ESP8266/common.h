#ifndef __COMMON_H__
#define __COMMON_H__	 
#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"   	 
#include "key.h"	 	 	 	 	 
#include "malloc.h"
#include "string.h"    		
#include "usart3.h" 
#include "stmflash.h"

#define STA 0x01
#define AP 0x02
#define AP_STA 0x03
#define ESP8266_SAVE_ADDR   0x8030000   //ESP8266配置信息保存地址

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
void atk_8266_init(void);

u8* atk_8266_check_cmd(u8 *str);
u8 atk_8266_apsta_check(void);
u8 atk_8266_consta_check(void);
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime);
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8 atk_8266_quit_trans(void);

u8 esp8266_init_sta(void);
u8 esp8266_init_ap(void);
u8 atk_8266_test(void);
void esp8266_set_mode(u8 mode);
u8 esp8266_join_ap(const u8 *wifista_ssid,const u8 *wifista_password);
u8 esp8266_establish_ap(const u8 *wifiap_ssid, const u8 *wifiap_password);
u8 esp8266_set_sta(u8 netpro, const u8 *ipbuf, const u8 *portnum);
u8 esp8266_set_ap(u8 netpro, const u8 *ipbuf, const u8 *portnum);

// USART3收到的数据进行解析
void send_data_to_usart3(u8 netpro, char *data);
// USART3收到的数据进行解析
void recv_data_analysis(u8 esp_flag, u8 *USART3_RX_BUF);

u8 atk_8266_apsta_test(void);	//WIFI AP+STA模式测试
u8 atk_8266_wifista_test(void);	//WIFI STA测试
u8 atk_8266_wifiap_test(void);	//WIFI AP测试

extern const u8* portnum;
 
extern const u8* wifista_ssid;
extern const u8* wifista_encryption;
extern const u8* wifista_password;

extern const u8* wifiap_ssid;
extern const u8* wifiap_encryption;
extern const u8* wifiap_password;

extern u8* kbd_fn_tbl[2];
extern const u8* ATK_ESP8266_CWMODE_TBL[3];
extern const u8* ATK_ESP8266_WORKMODE_TBL[3];
extern const u8* ATK_ESP8266_ECN_TBL[5];
#endif





