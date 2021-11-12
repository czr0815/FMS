#include "http.h"

const char online_head_json[] = 
	"Post /devices/setstatus HTTP/1.1\r\n"
  "Host: %s:%s\r\n"
  "Content-Type: application/json\r\n"
  "Content-Length: %d\r\n\r\n";

const char online_template_json[]=
	"{\"id\":\"%s\", \"status\":%d}";

const char data_head_json[]=
	"POST /data/add HTTP/1.1\r\n"
  "Host: %s:%s\r\n"
  "Content-Type: application/json\r\n"
  "Content-Length: %d\r\n\r\n";

const char data_template_json[]=
	"{\"did\":\"%s\", \"type\":\"%s\", \"value\":%f}";

int Post_Head_Package(const char *server_ip,const char *port_num,int ContentLength,char *out_data)
{
  int len = 0 ;
  char *up_head = NULL;
  if(NULL == server_ip || NULL == out_data)
		return -1;
  if(ContentLength < 0 || port_num < 0)
    return -2 ;
  up_head = mymalloc(SRAMIN, DATA_LEN);
  if(NULL != up_head)
	  snprintf(up_head,DATA_LEN,data_head_json,server_ip,port_num,ContentLength);
  if(NULL != out_data)
    sprintf(out_data,"%s",up_head);
  else
    return -3 ;
  len = strlen(up_head);
  myfree(SRAMIN, up_head);
  return len;
}

int Post_Data_Package(const char *id,const char *type,double value,char *out_data)
{
  int len = 0 ;
  char *up_data = NULL; 
  if(NULL == id || NULL == type || NULL == out_data)
    return -1 ;
  up_data = mymalloc(SRAMIN, DATA_LEN);
  if(NULL != up_data)
	  snprintf(up_data,DATA_LEN,data_template_json,id,type,value);
  if(NULL != out_data)
    sprintf(out_data,"%s",up_data);
  else
    return -2 ;
  len = strlen(up_data);
  myfree(SRAMIN, up_data);
  return len;
}

int Post_Online_Package(const char *id,uint8_t status,char *out_data)
{
  int len = 0 ;
  char *up_data = NULL; 
  if(NULL == id || NULL == out_data)
    return -1 ;
  up_data = mymalloc(SRAMIN, DATA_LEN);
  if(NULL != up_data)
	  snprintf(up_data,DATA_LEN,online_template_json,id,status);
  if(NULL != out_data)
    sprintf(out_data,"%s",up_data);
  else
    return -2 ;
  len = strlen(up_data);
  myfree(SRAMIN, up_data);
  return len;
}

/*
int Get_Head_Package(const char *server_ip,const char *port_num,char *id,uint8_t status,char *out_data)
{
  int len = 0 ;
  char *up_head = NULL;
  if(NULL == server_ip || NULL == out_data)
		return -1;
  if(port_num < 0)
    return -2 ;
  //up_head = mymalloc(SRAMIN, DATA_LEN);
  //if(NULL != up_head)
	snprintf(up_head,DATA_LEN,online_head,id,status,server_ip,port_num);
  if(NULL != out_data)
    sprintf(out_data,"%s",up_head);
  else
    return -3 ;
  len = strlen(up_head);
  //myfree(SRAMIN, up_head);
  return len;
}
*/

int Http_Data_Upload(const char *server_ip,const char *port_num,char *id,char *type,double value,char *out_buffer)
{
  int ContentLength = 0 ;
  char *http_head = NULL;
  char *http_data = NULL;
  char *upload_http_data = NULL;
  http_head = mymalloc(SRAMIN, DATA_LEN);
  if(NULL == http_head)
    return -1 ;
  http_data = mymalloc(SRAMIN, DATA_LEN);
  if(NULL == http_data)
    return -2 ;
  upload_http_data = mymalloc(SRAMIN, TOTAL_DATA_LEN);
  if(NULL == upload_http_data)
    return -3 ;
  ContentLength = Post_Data_Package(id,type,value,http_data);
  Post_Head_Package(server_ip,port_num,ContentLength,http_head);
  sprintf(upload_http_data,"%s%s",http_head,http_data);
  sprintf(out_buffer,"%s",upload_http_data);
  myfree(SRAMIN, http_head);
  myfree(SRAMIN, http_data);
  myfree(SRAMIN, upload_http_data);
  return 0 ;
}

int Http_Online_Upload(const char *server_ip, const char* port_num, char *id, uint8_t status, char *out_buffer){
	int ContentLength = 0 ;
  char *http_head = NULL;
  char *http_data = NULL;
  char *upload_http_data = NULL;
  http_head = mymalloc(SRAMIN, DATA_LEN);
  if(NULL == http_head)
    return -1 ;
  http_data = mymalloc(SRAMIN, DATA_LEN);
  if(NULL == http_data)
    return -2 ;
  upload_http_data = mymalloc(SRAMIN, TOTAL_DATA_LEN);
  if(NULL == upload_http_data)
    return -3 ;
  ContentLength = Post_Online_Package(id,status,http_data);
  Post_Head_Package(server_ip,port_num,ContentLength,http_head);
  sprintf(upload_http_data,"%s%s",http_head,http_data);
  sprintf(out_buffer,"%s",upload_http_data);
  myfree(SRAMIN, http_head);
  myfree(SRAMIN, http_data);
  myfree(SRAMIN, upload_http_data);
  return 0 ;
}
