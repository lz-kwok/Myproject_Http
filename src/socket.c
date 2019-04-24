#include     <stdio.h> 
#include     <stdlib.h>
#include     <stdint.h>
#include     <string.h>
#include     <unistd.h>
#include     <sys/types.h>
#include     <sys/stat.h>  
#include     <sys/socket.h>
#include     <sys/wait.h>
#include     <sys/time.h>
#include     <netinet/in.h>
#include     <arpa/inet.h>

#include     <errno.h>
#include     <signal.h>

#include     <fcntl.h>  
#include     <termios.h>
#include     <errno.h> 
#include     <pthread.h> 
#include     <sys/ioctl.h> 

#include     "../include/serialuart.h"
#include     "../include/systemrtc.h"
#include     "../include/socket.h"
#include     "../include/malloc.h"
#include     "../include/cjson.h"
#include     "../include/data.h"






#define IPSTR 		"118.190.136.20"
#define PORT 		8080
#define BUFSIZE 	1024

static char TimeString[20] = "20171017 16:00:00";



char *MakeJsonBody(AppStruct AppData)
{
    static uint8_t m = 0;

    cJSON * pJsonRoot = mymalloc(512*sizeof(cJSON *));
    cJSON * pSubJson = mymalloc(512*sizeof(cJSON *));
    cJSON * pBack = mymalloc(512*sizeof(cJSON *));
    char * p;

    for(m=0;m<8;m++)
        TimeString[m] = k30_storefile[m+16];

    TimeString[9]  = cTime[0];
	TimeString[10] = cTime[1];
    TimeString[12] = cTime[2];
	TimeString[13] = cTime[3];
	TimeString[15] = cTime[4];
	TimeString[16] = cTime[5];

    pJsonRoot = cJSON_CreateObject();
    if(NULL == pJsonRoot)
    {
        cJSON_Delete(pJsonRoot);
        return NULL;
    }

    cJSON_AddNumberToObject(pJsonRoot, "SN",AppData.Data.SerialNumber);
    cJSON_AddNumberToObject(pJsonRoot, "DeviceID",AppData.Data.TerminalInfoData.DeviceID);

    pSubJson = NULL;
    pSubJson = cJSON_CreateObject();

    if(NULL == pSubJson)
    {
      //create object faild, exit
      cJSON_Delete(pJsonRoot);

      return NULL;
    }

	cJSON_AddNumberToObject(pSubJson, "Temp",AppData.Data.AirStationData.Temp);
	cJSON_AddNumberToObject(pSubJson, "Humi",AppData.Data.AirStationData.Humidity);   
	cJSON_AddNumberToObject(pSubJson, "PM2_5",AppData.Data.AirStationData.PM25);
	cJSON_AddNumberToObject(pSubJson, "PM10",AppData.Data.AirStationData.PM1_0);
	cJSON_AddNumberToObject(pSubJson, "Press",AppData.Data.AirStationData.Press);
    cJSON_AddItemToObject(pJsonRoot, "AirData", pSubJson);

//    pBack = NULL;
    pBack = cJSON_CreateObject();

    if(NULL == pBack)
    {
      // create object faild, exit
      cJSON_Delete(pJsonRoot);

      return NULL;
    }

    cJSON_AddNumberToObject(pJsonRoot, "Version",AppData.Data.TerminalInfoData.Version);
    cJSON_AddNumberToObject(pJsonRoot, "Quanity",AppData.Data.TerminalInfoData.PowerQuanity);
    cJSON_AddStringToObject(pJsonRoot, "uptime",TimeString);

    p = cJSON_Print(pJsonRoot);
    if(NULL == p)
    {
      //convert json list to string faild, exit
      //because sub json pSubJson han been add to pJsonRoot, so just delete pJsonRoot, if you also delete pSubJson, it will coredump, and error is : double free
      cJSON_Delete(pJsonRoot);
      return NULL;
    }

    cJSON_Delete(pJsonRoot);

    return p;
}



void httPost(char *postdata)
{
    int sockfd, ret;
    struct sockaddr_in servaddr;
    char str1[4096], str2[4096], *str;
    socklen_t len;
    fd_set	 t_set1;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
                    printf("创建网络连接失败,本线程即将终止---socket error!\n");
                    exit(0);
    };

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, IPSTR, &servaddr.sin_addr) <= 0 ){
                    printf("创建网络连接失败,本线程即将终止--inet_pton error!\n");
                    exit(0);
    };

    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
                    printf("连接到服务器失败,connect error!\n");
                    exit(0);
    }

    printf("与远端建立了连接\n");
 
    //发送数据
    memset(str2, 0, 4096);
    strcat(str2,postdata);
    str=(char*)malloc(256);
    len =strlen(str2);
    sprintf(str,"%d", len);

    memset(str1, 0, 4096);
    strcat(str1,"POST /envm/Sensordata HTTP/1.1\n");
    strcat(str1,"Host:5d-x.cn:8080\n");
    strcat(str1,"Accept:*/*\n");
    strcat(str1,"Content-Type:text/html\n");
    strcat(str1,"Connection:close\n");
    strcat(str1,"Content-Length: ");
    strcat(str1, str);
    strcat(str1,"\n\n");

    strcat(str1, str2);
    strcat(str1,"\r\n\r\n");
    printf("%s\n",str1);

    ret = write(sockfd,str1,strlen(str1));
    if(ret < 0) {
            printf("发送失败！错误代码是%d，错误信息是'%s'\n",errno,strerror(errno));
            exit(0);
    }else{
            printf("消息发送成功，共发送了%d个字节！\n\n", ret);
    }

    usleep(2000);

    FD_ZERO(&t_set1);
    FD_SET(sockfd, &t_set1);

    close(sockfd);
}




