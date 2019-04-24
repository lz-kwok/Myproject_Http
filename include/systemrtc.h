#ifndef SYSTEMRTC_H
#define SYSTEMRTC_H

extern char k30_storefile[30];
extern char cTime[6];

struct rtc_time   
{   
	int tm_sec; 	//当前秒    
	int tm_min; 	//当前分钟    
	int tm_hour; 	//当前小时    
	int tm_mday; 	//当前在本月中的天，如11月1日，则为1    
	int tm_mon; 	//当前月，范围是0~11    
	int tm_year; 	//当前年和1900的差值，如2006年则为36    
	int tm_wday; 	//当前在本星期中的天，范围0~6    
	int tm_yday; 	//当前在本年中的天，范围0~365    
	int tm_isdst; 	//
};

int SetSystemTime(char *dt);
int SyncRTCRegulate(uint8_t *dt);
struct tm *getSystemTime(void);
void *Rtcthread(void *arg);


#endif