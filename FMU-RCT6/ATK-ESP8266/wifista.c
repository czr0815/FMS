#include "common.h"
#include "stdlib.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//ATK-ESP8266 WIFI模块 WIFI STA驱动代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2015/4/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
///////////////////////////////////////////////////////////////////////////////////////////////////////////

//ATK-ESP8266 WIFI STA测试
//用于测试TCP/UDP连接
//返回值:0,正常
//    其他,错误代码
u8 netpro=0;	//网络模式
u8 atk_8266_wifista_test(void)
{
    //u8 netpro=0;	//网络模式
    u8 key;
    u8 timex=0;
    u8 ipbuf[16] = "192.168.43.39"; 	// IP缓存
    u8 *p;
    u16 t=999;		//加速第一次获取链接状态
    u8 res=0;
    u16 rlen=0;
    u8 constate=0;	//连接状态
    p=mymalloc(SRAMIN,32);							//申请32字节内存
    atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//设置WIFI STA模式
    atk_8266_send_cmd("AT+RST","OK",20);		//DHCP服务器关闭(仅AP模式有效)
    delay_ms(1000);         //延时3S等待重启成功
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    //设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!!
    sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//设置无线参数:ssid,密码
    while(atk_8266_send_cmd(p,"WIFI GOT IP",300));					//连接目标路由器,并且获得IP
PRESTA:
	  //netpro|=atk_8266_netpro_sel(50,30,(u8*)ATK_ESP8266_CWMODE_TBL[0]);	//选择网络模式
		//netpro = 0x01;
    if(netpro&0X02)   //UDP
    {
        printf("%s","ATK-ESP WIFI-STA 测试");
        printf("%s","正在配置ATK-ESP模块,请稍等...");
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
            printf("%s","ATK-ESP WIFI-STA 测试");
            printf("%s","正在配置ATK-ESP模块,请稍等...");
            atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0：单连接，1：多连接
            sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标TCP服务器
            while(atk_8266_send_cmd(p,"OK",200))
            {
                printf("%s","WK_UP:返回重选");
                printf("%s","ATK-ESP 连接TCP Server失败"); //连接失败
                key=KEY_Scan(0);
                if(key==WKUP_PRES)goto PRESTA;
            }
            atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //传输模式为：透传
        }
        else					//TCP Server
        {
            printf("%s","ATK-ESP WIFI-STA 测试");
            printf("%s","正在配置ATK-ESP模块,请稍等...");
            atk_8266_send_cmd("AT+CIPMUX=1","OK",20);   //0：单连接，1：多连接
            sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);    //开启Server模式(0，关闭；1，打开)，端口号为portnum
            atk_8266_send_cmd(p,"OK",50);
        }
    }
    printf("%s","ATK-ESP WIFI-STA 测试");
    printf("%s","正在配置ATK-ESP模块,请稍等...");
    printf("%s","WK_UP:退出测试  KEY0:发送数据");
    //atk_8266_get_wanip(ipbuf);//服务器模式,获取WAN IP
    sprintf((char*)p,"IP地址:%s 端口:%s",ipbuf,(u8*)portnum);
    printf("%s",p);				//显示IP地址和端口
    printf("%s","状态:"); 		//连接状态
    printf("%s","模式:"); 		//连接状态
    printf("%s","发送数据:"); 	//发送数据
    printf("%s","接收数据:");	//接收数据
    //atk_8266_wificonf_show(30,180,"请设置路由器无线参数为:",(u8*)wifista_ssid,(u8*)wifista_encryption,(u8*)wifista_password);
    printf("%s",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro]); 		//连接状态
    USART3_RX_STA=0;
    while(1)
    {
        key=KEY_Scan(0);
        if(key==WKUP_PRES)			//WK_UP 退出测试
        {
            res=0;
            atk_8266_quit_trans();	//退出透传
            atk_8266_send_cmd("AT+CIPMODE=0","OK",20);   //关闭透传模式
            break;
        }
        else if(key==KEY0_PRES)	//KEY0 发送数据
        {

            if((netpro==3)||(netpro==2))   //UDP
            {
                sprintf((char*)p,"ATK-8266%s测试%02d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//测试数据
                printf("%s",p);
                atk_8266_send_cmd("AT+CIPSEND=25","OK",200);  //发送指定长度的数据
                delay_ms(200);
                atk_8266_send_data(p,"OK",100);  //发送指定长度的数据
                timex=100;
            }
            else if((netpro==1))   //TCP Client
            {
                atk_8266_quit_trans();
                atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传
                sprintf((char*)p,"ATK-8266%s测试%d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//测试数据
                printf("%s",p);
                u3_printf("%s",p);
                timex=100;
            }
            else    //TCP Server
            {
                sprintf((char*)p,"ATK-8266%s测试%02d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//测试数据
                printf("%s",p);
                atk_8266_send_cmd("AT+CIPSEND=0,25","OK",200);  //发送指定长度的数据
                delay_ms(200);
                atk_8266_send_data(p,"OK",100);  //发送指定长度的数据
                timex=100;
            }
        } else;

        if(timex)timex--;
        t++;
        delay_ms(10);
        if(USART3_RX_STA&0X8000)		//接收到一次数据了
        {
            rlen=USART3_RX_STA&0X7FFF;	//得到本次接收到的数据长度
            USART3_RX_BUF[rlen]=0;		//添加结束符
            sprintf((char*)p,"收到%d字节,内容如下",rlen);//接收到的字节数
            printf("%s\r\n",p); 			//显示接收到的数据长度
            printf("接收数据:%s\r\n",USART3_RX_BUF);//显示接收到的数据
            USART3_RX_STA=0;
			
			// USART3收到的数据进行解析
			recv_data_analysis(netpro, USART3_RX_BUF);
			
            if(constate!='+')t=1000;		//状态为还未连接,立即更新连接状态
            else t=0;                   //状态为已经连接了,10秒后再检查
        }
        if(t==1000)//连续10秒钟没有收到任何数据,检查连接是不是还存在.
        {
//			//		LCD_Fill(30+54,125,239,130,WHITE);
//					LCD_Fill(60,80,120,92,WHITE);
            constate=atk_8266_consta_check();//得到连接状态
            if(constate=='+')printf("%s","连接成功");  //连接状态
            else printf("%s","连接失败");
            t=0;
        }
        if((t%20)==0)LED0=!LED0;
        //atk_8266_at_response(1);
    }
    myfree(SRAMIN,p);		//释放内存
    return res;
}




























