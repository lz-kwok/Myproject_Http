#include     <unistd.h>
#include     <stdio.h> 
#include     <stdlib.h>
#include     <stdint.h>
#include     <sys/time.h>
#include     <time.h>
#include     "../include/socket.h"
#include     "../include/systemrtc.h"
#include     "../include/malloc.h"
#include     "../include/serialuart.h"
#include     "../include/data.h"



char k30_storefile[30] = "/tfCard/CO2/k30_          .txt";
char cTime[6] = "000000";
struct tm* systemtime;


static uint8_t BCD2HEX(uint8_t bcd_data)    //BCD转为HEX子程序    
{   
    uint8_t temp;   
    temp=(bcd_data/16*10 + bcd_data%16);   
	
    return temp;   
} 



int SetSystemTime(char *dt)  
{  
    struct rtc_time tm;  
    struct tm _tm;  
    struct timeval tv;  
    time_t timep;  
    sscanf(dt, "%d-%d-%d %d:%d:%d", &tm.tm_year,  
        &tm.tm_mon, &tm.tm_mday,&tm.tm_hour,  
        &tm.tm_min, &tm.tm_sec);  
    _tm.tm_sec = tm.tm_sec;  
    _tm.tm_min = tm.tm_min;  
    _tm.tm_hour = tm.tm_hour;  
    _tm.tm_mday = tm.tm_mday;  
    _tm.tm_mon = tm.tm_mon - 1;  
    _tm.tm_year = tm.tm_year - 1900;  
  
    timep = mktime(&_tm);  
    tv.tv_sec = timep;  
    tv.tv_usec = 0;  
	
    if(settimeofday (&tv, (struct timezone *) 0) < 0){  
	    printf("Set system datatime error!/n");  
	    return -1;  
    }  
	
    return 0;  
} 

//通过串口进行时钟同步
int SyncRTCRegulate(uint8_t *dt)  
{  
    struct tm _tm;  
    struct timeval tv;  
    time_t timep;  

    _tm.tm_sec  = BCD2HEX(dt[6]);  
    _tm.tm_min  = BCD2HEX(dt[5]); 
    _tm.tm_hour = BCD2HEX(dt[4]);  
    _tm.tm_mday = BCD2HEX(dt[3]);   
    _tm.tm_mon  = BCD2HEX(dt[2]) - 1;  
    _tm.tm_year = BCD2HEX(dt[1]) + 2000 - 1900;    
  
    timep = mktime(&_tm);  
    tv.tv_sec = timep;  
    tv.tv_usec = 0;  
	
    if(settimeofday (&tv, (struct timezone *) 0) < 0){  
	    printf("Set system datatime error!/n");  
	    return -1;  
    }else
		printf("Set system datatime done!/n"); 
	
    return 0;  
}



struct tm *getSystemTime(void)   
{   
	time_t timer;   
	struct tm* t_tm;   
	time(&timer);   
	t_tm = localtime(&timer);   

//	printf("today is %4d%02d%02d%02d%02d%02d\n", t_tm->tm_year+1900,   
//	t_tm->tm_mon+1, t_tm->tm_mday, t_tm->tm_hour, t_tm->tm_min, t_tm->tm_sec);   
	
	return t_tm;   
}  


void *Rtcthread(void *arg)
{
	struct tm* sT;
	char filecache[4];
	while(1){
		sT = getSystemTime();   
//		printf("today is %4d%02d%02d%02d%02d%02d\n", sT->tm_year+1900,   
//				sT->tm_mon+1, sT->tm_mday, sT->tm_hour, sT->tm_min, sT->tm_sec); 

		
		sprintf(filecache,"%4d",sT->tm_year+1900);		
		k30_storefile[16] = filecache[0];
		k30_storefile[17] = filecache[1];
		k30_storefile[18] = filecache[2];
		k30_storefile[19] = filecache[3];
		sprintf(filecache,"%02d",sT->tm_mon+1);
		k30_storefile[20] = filecache[0];
		k30_storefile[21] = filecache[1];
		sprintf(filecache,"%02d",sT->tm_mday);
		k30_storefile[22] = filecache[0];
		k30_storefile[23] = filecache[1];
		sprintf(filecache,"%02d",sT->tm_hour);
		cTime[0] = k30_storefile[24] = filecache[0];
		cTime[1] = k30_storefile[25] = filecache[1];
		sprintf(filecache,"%02d",sT->tm_min);
		cTime[2] = filecache[0];
		cTime[3] = filecache[1];
		sprintf(filecache,"%02d",sT->tm_sec);
		cTime[4] = filecache[0];
		cTime[5] = filecache[1];

		if((sT->tm_min%2 == 0)&&(sT->tm_sec == 5)){
//			printf("file is %s\n",k30_storefile);
			mallco_dev.init();

			char * jsonBody = mymalloc(512*sizeof(char *));

			App.Data.SerialNumber += 1;
			App.Data.TerminalInfoData.DeviceID += 1;
			
			if(App.Data.TerminalInfoData.DeviceID == 30040){
				App.Data.SerialNumber = 30028;
				App.Data.TerminalInfoData.DeviceID = 30028;
				}

			
			jsonBody = MakeJsonBody(App);
			printf("%s\n",jsonBody);
			httPost(jsonBody);
			myfree(jsonBody);
			}

		sleep(1);				
		}
}




