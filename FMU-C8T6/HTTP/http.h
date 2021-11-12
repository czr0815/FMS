#include "sys.h"
#include "stdio.h"
#include "string.h"
#include "malloc.h"

#define TOTAL_DATA_LEN 1020
#define MAX_DATA_LEN 2048
#define DATA_LEN 512

int Http_Data_Upload(const char *server_ip,const char *port_num,char *id,char *type,double value,char *out_buffer);
int Http_Online_Upload(const char *server_ip,const char *port_num,char *id,uint8_t status,char *out_buffer); 
