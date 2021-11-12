#include "common.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//�û�������

const u8* portnum="";

//WIFISTAģʽ��TCP͸������Ŀ��IP�Ͷ˿ں�.
const u8* wifista_ipbuf="1.117.233.138";
const u8* wifista_portnum="8888";

//WIFIAPģʽ��������IP�Ͷ˿ں�.
const u8* wifiap_ipbuf="192.168.4.1";
const u8* wifiap_portnum="8086";

//WIFI STAģʽ,����Ҫȥ���ӵ�·�������߲���,��������Լ���·��������,�����޸�.
//const u8* wifista_ssid="sleepy";			//·����SSID��
const u8* wifista_ssid="sleepy";
const u8* wifista_encryption="wpawpa2_aes";	//wpa/wpa2 aes���ܷ�ʽ
const u8* wifista_password="czr6025645"; 	//��������

//WIFI APģʽ,ģ���������߲���,�������޸�.
const u8* wifiap_ssid="FMU";			//����SSID��
const u8* wifiap_encryption="wpawpa2_aes";	//wpa/wpa2 aes���ܷ�ʽ
const u8* wifiap_password="smartfishing"; 		//�������� 

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//4������ģʽ
const u8 *ATK_ESP8266_CWMODE_TBL[3]={"STAģʽ ","APģʽ ","AP&STAģʽ "};	//ATK-ESP8266,3������ģʽ,Ĭ��Ϊ·����(ROUTER)ģʽ 
//4�ֹ���ģʽ
const u8 *ATK_ESP8266_WORKMODE_TBL[3]={"TCP������","TCP�ͻ���"," UDP ģʽ"};	//ATK-ESP8266,4�ֹ���ģʽ
//5�ּ��ܷ�ʽ
const u8 *ATK_ESP8266_ECN_TBL[5]={"OPEN","WEP","WPA_PSK","WPA2_PSK","WPA_WAP2_PSK"};
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

//ATK-ESP8266���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
u8* atk_8266_check_cmd(u8 *str)
{
	
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//��ATK-ESP8266��������
//cmd:���͵������ַ���
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s\r\n",cmd);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(atk_8266_check_cmd(ack))
				{
					//printf("ack:%s\r\n",(u8*)ack);
					break;//�õ���Ч���� 
				}
					USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
//��ATK-ESP8266����ָ������
//data:���͵�����(����Ҫ��ӻس���)
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)luojian
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s",data);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(atk_8266_check_cmd(ack))break;//�õ���Ч���� 
				USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}
//ATK-ESP8266�˳�͸��ģʽ
//����ֵ:0,�˳��ɹ�;
//       1,�˳�ʧ��
u8 atk_8266_quit_trans(void)
{
	while((USART3->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='+';      
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART3->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='+';      
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART3->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='+';      
	delay_ms(500);					//�ȴ�500ms
	return atk_8266_send_cmd("AT","OK",20);//�˳�͸���ж�.
}
//��ȡATK-ESP8266ģ���AP+STA����״̬
//����ֵ:0��δ����;1,���ӳɹ�
u8 atk_8266_apsta_check(void)
{
	if(atk_8266_quit_trans())return 0;			//�˳�͸�� 
	atk_8266_send_cmd("AT+CIPSTATUS",":",50);	//����AT+CIPSTATUSָ��,��ѯ����״̬
	if(atk_8266_check_cmd("+CIPSTATUS:0")&&
		 atk_8266_check_cmd("+CIPSTATUS:1")&&
		 atk_8266_check_cmd("+CIPSTATUS:2")&&
		 atk_8266_check_cmd("+CIPSTATUS:4"))
		return 0;
	else return 1;
}
//��ȡATK-ESP8266ģ�������״̬
//����ֵ:0,δ����;1,���ӳɹ�.
u8 atk_8266_consta_check(void)
{
	u8 *p;
	u8 res;
	if(atk_8266_quit_trans())return 0;			//�˳�͸�� 
	atk_8266_send_cmd("AT+CIPSTATUS",":",50);	//����AT+CIPSTATUSָ��,��ѯ����״̬
	p=atk_8266_check_cmd("+CIPSTATUS:"); 
	res=*p;									//�õ�����״̬	
	return res;
}

//ATK-ESP8266ģ�����������
u8 atk_8266_test(void)
{
//	u16 rlen=0;
	u8 key;
	u8 timex = 5;
	printf("ATK-ESP8266 WIFIģ�����\r\n"); 
	while(atk_8266_send_cmd("AT","OK",200)&&timex)//���WIFIģ���Ƿ�����
	{
		atk_8266_quit_trans();//�˳�͸��
		atk_8266_send_cmd("AT+CIPMODE=0","OK",200);  //�ر�͸��ģʽ	
		printf("δ��⵽ģ��!!!\r\n");
		delay_ms(800);
		printf("��������ģ��...\r\n"); 
		timex--;
	}
	if(!timex) return 1;
	timex = 5;
	while(atk_8266_send_cmd("ATE0","OK",20)&&timex){//�رջ���
		timex--;
		delay_ms(800);
	}
	if(!timex) return 1;
	return 0;
	//atk_8266_mtest_ui(32,30);
}

//���ù���ģʽ�������г�ʼ��
void esp8266_set_mode(u8 mode){
	printf("����ESP8266����ģʽ...\r\n");
	if(mode==STA){
		atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//����WIFI STAģʽ
    atk_8266_send_cmd("AT+RST","OK",20);		//DHCP�������ر�(��APģʽ��Ч)
    delay_ms(1000);         //��ʱ3S�ȴ������ɹ�
    delay_ms(1000);
    delay_ms(1000);
	}else if(mode==AP){
		atk_8266_send_cmd("AT+CWMODE=2","OK",50);
		atk_8266_send_cmd("AT+RST","OK",20);
		delay_ms(1000);         //��ʱ3S�ȴ������ɹ�
    delay_ms(1000);
    delay_ms(1000);
	}else if(mode==AP_STA){
		atk_8266_send_cmd("AT+CWMODE=3","OK",50);
		atk_8266_send_cmd("AT+RST","OK",20);
		delay_ms(1000);         //��ʱ3S�ȴ������ɹ�
    delay_ms(1000);
    delay_ms(1000);
	}
}

//staģʽ������ָ��ap
u8 esp8266_join_ap(const u8 *wifista_ssid, const u8 *wifista_password){
	u8 *p=mymalloc(SRAMIN,32),timex = 8;
	printf("����ָ���ȵ�...\r\n");
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//�������߲���:ssid,����
  while(atk_8266_send_cmd(p,"OK",300)&&timex){//����Ŀ��·����,���һ��IP
		timex--;
		delay_ms(800);
	}
	myfree(SRAMIN, p);
	if(!timex) return 1;
	return 0;
}

u8 esp8266_establish_ap(const u8 *wifiap_ssid, const u8 *wifiap_password){
	u8 *p=mymalloc(SRAMIN,32),timex = 8;
	printf("�����ȵ�...\r\n");
	sprintf((char*)p,"AT+CWSAP=\"%s\",\"%s\",1,4",wifiap_ssid,wifiap_password);//�������߲���:ssid,����
  while(atk_8266_send_cmd(p,"OK",1000)&&timex){//����APģʽ����
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
    printf("����WIFI-STA:UDPģʽ...\r\n");
    sprintf((char*)p,"AT+CIPSTART=\"UDP\",\"%s\",%s",ipbuf,(u8*)portnum);    //����Ŀ��UDP������
    delay_ms(200);
    atk_8266_send_cmd("AT+CIPMUX=0","OK",20);  //������ģʽ
    delay_ms(200);
    while(atk_8266_send_cmd(p,"OK",500));
  }
  else     //TCP
  {
    if(netpro&0X01)     //TCP Client    ͸��ģʽ����
    {
      printf("����WIFI-STA:TCP Client͸��ģʽ...\r\n");
      atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0�������ӣ�1��������
      sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //����Ŀ��TCP������
      while(atk_8266_send_cmd(p,"OK",200)&&timex)
      {
				timex--;
        printf("%s\r\n","����WIFI-STA:TCP Client͸��ģʽ����TCP Serverʧ��"); //����ʧ��
      }
			if(!timex) return 1;
      atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //����ģʽΪ��͸��
    }
    else					//TCP Server
    {
      printf("����WIFI-STA:TCP Serverģʽ...\r\n");
      atk_8266_send_cmd("AT+CIPMUX=1","OK",20);   //0�������ӣ�1��������
      sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);    //����Serverģʽ(0���رգ�1����)���˿ں�Ϊportnum
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
		printf("����WIFI-AP:UDPģʽ...\r\n");
		sprintf((char*)p,"AT+CIPSTART=\"UDP\",\"%s\",%s",ipbuf,(u8*)portnum);    //����Ŀ��UDP������
    atk_8266_send_cmd("AT+CIPMUX=0","OK",100);  //������ģʽ
    while(atk_8266_send_cmd(p,"OK",500));
	}
  else     //TCP
  {
    if(netpro&0X01)     //TCP Client    ͸��ģʽ����
    {
			printf("����WIFI-AP:TCP Client͸��ģʽ...\r\n");
      atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0�������ӣ�1��������
      sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //����Ŀ��TCP������
      while(atk_8266_send_cmd(p,"OK",200))
      {
				printf("%s\r\n","����WIFI-AP:TCP Client͸��ģʽ����TCP Serverʧ��"); //����ʧ��
      }
      atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //����ģʽΪ��͸��
    }
    else					//TCP Server
    {
			printf("����WIFI-AP:TCP Serverģʽ...\r\n");
      atk_8266_send_cmd("AT+CIPMUX=1","OK",20);   //0�������ӣ�1��������
      sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);
      atk_8266_send_cmd(p,"OK",20);     //����Serverģʽ���˿ں�Ϊ8086
    }
  }
	myfree(SRAMIN, p);
	return 0;
}

//���ⷢ������
void send_data_to_usart3(u8 netpro, char *data)
{
	u8 len = 0;
	u8 buf[20] = {0};
	
	len = strlen(data);
	
	if((netpro==3)||(netpro==2))   //UDP
	{
		sprintf((char*)buf, "AT+CIPSEND=%d", len);
		atk_8266_send_cmd(buf,"OK",200);  //����ָ�����ȵ�����
		delay_ms(200);
		atk_8266_send_data((u8 *)data,"OK",100);  //����ָ�����ȵ�����
	}
	else if(netpro==1)   //TCP Client
	{
		atk_8266_quit_trans();
		atk_8266_send_cmd("AT+CIPSEND","OK",20);         //��ʼ͸��
		u3_printf("%s", data);
	}
	else // TCP server
	{
		sprintf((char*)buf, "AT+CIPSEND=0,%d", len);
		atk_8266_send_cmd(buf,"OK",200);  //����ָ�����ȵ�����
		delay_ms(200);
		atk_8266_send_data((u8 *)data,"OK",100);  //����ָ�����ȵ�����
	}
}

// USART3�յ������ݽ��н���
void recv_data_analysis(u8 esp_flag, u8 *USART3_RX_BUF)
{
	char *p = NULL;
	p = mymalloc(SRAMIN, 100); //����100�ֽ��ڴ�
	if(!esp_flag){
		//STAģʽ��
		sprintf(p, "%s", USART3_RX_BUF);
	}
	else{
		//APģʽ��
		char *str1;
		u16 size;
		sprintf(p, "%s", USART3_RX_BUF);
		//printf("�յ����ݣ�%s\r\n",p);
		strtok(p,":");
		p = strtok(NULL,":");
		//printf("p��%s\r\n",p);
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
	myfree(SRAMIN, p);		// �ͷ��ڴ�
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
	p = mymalloc(SRAMIN, size*2); //����100�ֽ��ڴ�
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
