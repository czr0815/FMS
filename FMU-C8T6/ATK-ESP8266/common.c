#include "common.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//用户配置区

const u8* portnum="";

//WIFISTA模式，TCP透传连接目标IP和端口号.
const u8* wifista_ipbuf="1.117.233.138";
const u8* wifista_portnum="8888";

//WIFIAP模式，构建的IP和端口号.
const u8* wifiap_ipbuf="192.168.4.1";
const u8* wifiap_portnum="8086";

//WIFI STA模式,设置要去连接的路由器无线参数,请根据你自己的路由器设置,自行修改.
//const u8* wifista_ssid="sleepy";			//路由器SSID号
const u8* wifista_ssid="sleepy";
const u8* wifista_encryption="wpawpa2_aes";	//wpa/wpa2 aes加密方式
const u8* wifista_password="czr6025645"; 	//连接密码

//WIFI AP模式,模块对外的无线参数,可自行修改.
const u8* wifiap_ssid="FMU";			//对外SSID号
const u8* wifiap_encryption="wpawpa2_aes";	//wpa/wpa2 aes加密方式
const u8* wifiap_password="smartfishing"; 		//连接密码 

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//4个网络模式
const u8 *ATK_ESP8266_CWMODE_TBL[3]={"STA模式 ","AP模式 ","AP&STA模式 "};	//ATK-ESP8266,3种网络模式,默认为路由器(ROUTER)模式 
//4种工作模式
const u8 *ATK_ESP8266_WORKMODE_TBL[3]={"TCP服务器","TCP客户端"," UDP 模式"};	//ATK-ESP8266,4种工作模式
//5种加密方式
const u8 *ATK_ESP8266_ECN_TBL[5]={"OPEN","WEP","WPA_PSK","WPA2_PSK","WPA_WAP2_PSK"};
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

//ATK-ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* atk_8266_check_cmd(u8 *str)
{
	
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//向ATK-ESP8266发送命令
//cmd:发送的命令字符串
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s\r\n",cmd);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(atk_8266_check_cmd(ack))
				{
					//printf("ack:%s\r\n",(u8*)ack);
					break;//得到有效数据 
				}
					USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
//向ATK-ESP8266发送指定数据
//data:发送的数据(不需要添加回车了)
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)luojian
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s",data);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(atk_8266_check_cmd(ack))break;//得到有效数据 
				USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}
//ATK-ESP8266退出透传模式
//返回值:0,退出成功;
//       1,退出失败
u8 atk_8266_quit_trans(void)
{
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(500);					//等待500ms
	return atk_8266_send_cmd("AT","OK",20);//退出透传判断.
}
//获取ATK-ESP8266模块的AP+STA连接状态
//返回值:0，未连接;1,连接成功
u8 atk_8266_apsta_check(void)
{
	if(atk_8266_quit_trans())return 0;			//退出透传 
	atk_8266_send_cmd("AT+CIPSTATUS",":",50);	//发送AT+CIPSTATUS指令,查询连接状态
	if(atk_8266_check_cmd("+CIPSTATUS:0")&&
		 atk_8266_check_cmd("+CIPSTATUS:1")&&
		 atk_8266_check_cmd("+CIPSTATUS:2")&&
		 atk_8266_check_cmd("+CIPSTATUS:4"))
		return 0;
	else return 1;
}
//获取ATK-ESP8266模块的连接状态
//返回值:0,未连接;1,连接成功.
u8 atk_8266_consta_check(void)
{
	u8 *p;
	u8 res;
	if(atk_8266_quit_trans())return 0;			//退出透传 
	atk_8266_send_cmd("AT+CIPSTATUS",":",50);	//发送AT+CIPSTATUS指令,查询连接状态
	p=atk_8266_check_cmd("+CIPSTATUS:"); 
	res=*p;									//得到连接状态	
	return res;
}

//ATK-ESP8266模块测试主函数
u8 atk_8266_test(void)
{
//	u16 rlen=0;
	u8 key;
	u8 timex = 5;
	printf("ATK-ESP8266 WIFI模块测试\r\n"); 
	while(atk_8266_send_cmd("AT","OK",200)&&timex)//检查WIFI模块是否在线
	{
		atk_8266_quit_trans();//退出透传
		atk_8266_send_cmd("AT+CIPMODE=0","OK",200);  //关闭透传模式	
		printf("未检测到模块!!!\r\n");
		delay_ms(800);
		printf("尝试连接模块...\r\n"); 
		timex--;
	}
	if(!timex) return 1;
	timex = 5;
	while(atk_8266_send_cmd("ATE0","OK",20)&&timex){//关闭回显
		timex--;
		delay_ms(800);
	}
	if(!timex) return 1;
	return 0;
	//atk_8266_mtest_ui(32,30);
}

//设置工作模式，并进行初始化
void esp8266_set_mode(u8 mode){
	printf("设置ESP8266工作模式...\r\n");
	if(mode==STA){
		atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//设置WIFI STA模式
    atk_8266_send_cmd("AT+RST","OK",20);		//DHCP服务器关闭(仅AP模式有效)
    delay_ms(1000);         //延时3S等待重启成功
    delay_ms(1000);
    delay_ms(1000);
	}else if(mode==AP){
		atk_8266_send_cmd("AT+CWMODE=2","OK",50);
		atk_8266_send_cmd("AT+RST","OK",20);
		delay_ms(1000);         //延时3S等待重启成功
    delay_ms(1000);
    delay_ms(1000);
	}else if(mode==AP_STA){
		atk_8266_send_cmd("AT+CWMODE=3","OK",50);
		atk_8266_send_cmd("AT+RST","OK",20);
		delay_ms(1000);         //延时3S等待重启成功
    delay_ms(1000);
    delay_ms(1000);
	}
}

//sta模式下连接指定ap
u8 esp8266_join_ap(const u8 *wifista_ssid, const u8 *wifista_password){
	u8 *p=mymalloc(SRAMIN,32),timex = 8;
	printf("连接指定热点...\r\n");
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//设置无线参数:ssid,密码
  while(atk_8266_send_cmd(p,"OK",300)&&timex){//连接目标路由器,并且获得IP
		timex--;
		delay_ms(800);
	}
	myfree(SRAMIN, p);
	if(!timex) return 1;
	return 0;
}

u8 esp8266_establish_ap(const u8 *wifiap_ssid, const u8 *wifiap_password){
	u8 *p=mymalloc(SRAMIN,32),timex = 8;
	printf("配置热点...\r\n");
	sprintf((char*)p,"AT+CWSAP=\"%s\",\"%s\",1,4",wifiap_ssid,wifiap_password);//设置无线参数:ssid,密码
  while(atk_8266_send_cmd(p,"OK",1000)&&timex){//设置AP模式参数
		timex--;
	}
	myfree(SRAMIN, p);
	if(!timex) return 1;
	return 0;
}

u8 esp8266_set_sta(u8 netpro, const u8 *ipbuf, const u8 *portnum){
	u8 *p=mymalloc(SRAMIN,32),timex = 5;
  if(netpro&0X02)   //UDP
  {
    printf("设置WIFI-STA:UDP模式...\r\n");
    sprintf((char*)p,"AT+CIPSTART=\"UDP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标UDP服务器
    delay_ms(200);
    atk_8266_send_cmd("AT+CIPMUX=0","OK",20);  //单链接模式
    delay_ms(200);
    while(atk_8266_send_cmd(p,"OK",500));
  }
  else     //TCP
  {
    if(netpro&0X01)     //TCP Client    透传模式测试
    {
      printf("设置WIFI-STA:TCP Client透传模式...\r\n");
      atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0：单连接，1：多连接
      sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标TCP服务器
      while(atk_8266_send_cmd(p,"OK",200)&&timex)
      {
				timex--;
        printf("%s\r\n","设置WIFI-STA:TCP Client透传模式连接TCP Server失败"); //连接失败
      }
			if(!timex) return 1;
      atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //传输模式为：透传
    }
    else					//TCP Server
    {
      printf("设置WIFI-STA:TCP Server模式...\r\n");
      atk_8266_send_cmd("AT+CIPMUX=1","OK",20);   //0：单连接，1：多连接
      sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);    //开启Server模式(0，关闭；1，打开)，端口号为portnum
      atk_8266_send_cmd(p,"OK",50);
    }
  }
	myfree(SRAMIN, p);
	return 0;
}

u8 esp8266_set_ap(u8 netpro, const u8 *ipbuf, const u8 *portnum){
	u8 *p=mymalloc(SRAMIN,32);
	if(netpro&0X02)   //UDP
  {
		printf("设置WIFI-AP:UDP模式...\r\n");
		sprintf((char*)p,"AT+CIPSTART=\"UDP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标UDP服务器
    atk_8266_send_cmd("AT+CIPMUX=0","OK",100);  //单链接模式
    while(atk_8266_send_cmd(p,"OK",500));
	}
  else     //TCP
  {
    if(netpro&0X01)     //TCP Client    透传模式测试
    {
			printf("设置WIFI-AP:TCP Client透传模式...\r\n");
      atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0：单连接，1：多连接
      sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标TCP服务器
      while(atk_8266_send_cmd(p,"OK",200))
      {
				printf("%s\r\n","设置WIFI-AP:TCP Client透传模式连接TCP Server失败"); //连接失败
      }
      atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //传输模式为：透传
    }
    else					//TCP Server
    {
			printf("设置WIFI-AP:TCP Server模式...\r\n");
      atk_8266_send_cmd("AT+CIPMUX=1","OK",20);   //0：单连接，1：多连接
      sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);
      atk_8266_send_cmd(p,"OK",20);     //开启Server模式，端口号为8086
    }
  }
	myfree(SRAMIN, p);
	return 0;
}

//对外发送数据
void send_data_to_usart3(u8 netpro, char *data)
{
	u8 len = 0;
	u8 buf[20] = {0};
	
	len = strlen(data);
	
	if((netpro==3)||(netpro==2))   //UDP
	{
		sprintf((char*)buf, "AT+CIPSEND=%d", len);
		atk_8266_send_cmd(buf,"OK",200);  //发送指定长度的数据
		delay_ms(200);
		atk_8266_send_data((u8 *)data,"OK",100);  //发送指定长度的数据
	}
	else if(netpro==1)   //TCP Client
	{
		atk_8266_quit_trans();
		atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传
		u3_printf("%s", data);
	}
	else // TCP server
	{
		sprintf((char*)buf, "AT+CIPSEND=0,%d", len);
		atk_8266_send_cmd(buf,"OK",200);  //发送指定长度的数据
		delay_ms(200);
		atk_8266_send_data((u8 *)data,"OK",100);  //发送指定长度的数据
	}
}

// USART3收到的数据进行解析
void recv_data_analysis(u8 esp_flag, u8 *USART3_RX_BUF)
{
	char *p = NULL;
	p = mymalloc(SRAMIN, 100); //申请100字节内存
	if(!esp_flag){
		//STA模式下
		sprintf(p, "%s", USART3_RX_BUF);
	}
	else{
		//AP模式下
		char *str1;
		u16 size;
		sprintf(p, "%s", USART3_RX_BUF);
		//printf("收到数据：%s\r\n",p);
		strtok(p,":");
		p = strtok(NULL,":");
		//printf("p：%s\r\n",p);
		str1 = strstr(p, "||");
		if(str1 == NULL){
			myfree(SRAMIN, p);
			return;
		}
		size = strlen(p);
		//printf("size:%d\r\n",size);
		Test_Write(ESP8266_SAVE_ADDR,size);
		STMFLASH_Write(ESP8266_SAVE_ADDR+2,(u16*)p,size);
	}
	myfree(SRAMIN, p);		// 释放内存
}

u8 esp8266_init_sta(){
	u16 size=0;
	char *p, *str1, *str2;
	if(atk_8266_test()){
    return 0;		
	}
	esp8266_set_mode(STA);
	
	STMFLASH_Read(ESP8266_SAVE_ADDR, &size, 1);
	//printf("size:%d\r\n",size);
	p = mymalloc(SRAMIN, size*2); //申请100字节内存
	STMFLASH_Read(ESP8266_SAVE_ADDR+2, (u16*)p, size);
	//printf("p:%s\r\n",p);
	str1 = strtok(p,"||");
	str2 = strtok(NULL,"||");
	printf("wifista_ssid:%s, wifista_password:%s\r\n",str1,str2);
	if(esp8266_join_ap(str1, str2)){
		myfree(SRAMIN, p);
		return 0;
	}
	if(esp8266_set_sta(0x01, wifista_ipbuf, wifista_portnum)){
		myfree(SRAMIN, p);
		return 0;
	}
	myfree(SRAMIN, p);
	return 1;
}

u8 esp8266_init_ap(){
	if(atk_8266_test()){
		return 0;
	}
	esp8266_set_mode(AP);
	if(esp8266_establish_ap(wifiap_ssid, wifiap_password)){
		return 0;
	}
	if(esp8266_set_ap(0x00, wifiap_ipbuf, wifiap_portnum)){
		return 0;
	}
	return 1;
}
