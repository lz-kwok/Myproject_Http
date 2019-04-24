
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
#include     <time.h>
#include     "../include/serialuart.h"
#include     "../include/systemrtc.h"
#include     "../include/socket.h"



static void InitData(void)
{
//    AppDataPointer = &(App.Data);
//    AppPointer     = &App;

    App.Data.BlackRiverData.CODValue       = 0.0;
    App.Data.BlackRiverData.TemValue       = 0.0;
    App.Data.BlackRiverData.DORealValue    = 0.0;
    App.Data.BlackRiverData.NH4Value       = 0.0;
    App.Data.BlackRiverData.DoPercent      = 0.0;
    App.Data.BlackRiverData.ORPValue       = 0.0;
    App.Data.BlackRiverData.ZSValue        = 0.0;

    App.Data.AirStationData.WindSpeed      = 0.0;
    App.Data.AirStationData.WindDirection  = 0.0;
    App.Data.AirStationData.Humidity       = 0.0;
    App.Data.AirStationData.Temp           = 0.0;
    App.Data.AirStationData.PM25           = 0.0;

    //终端信息
    App.Data.TerminalInfoData.Version      = 100;
    App.Data.TerminalInfoData.PowerQuanity = 100;
    App.Data.TerminalInfoData.DeviceID     = 30029;      //设备ID
    App.Data.SerialNumber = 30029;
}





int main(int argc, char *argv[])
{
    pthread_t transmitid;
    pthread_t rtcid;
    int  cmd=0;
	InitData();
	if (!cmd){
		int temp;
		printf("create Uartthread\n");  
		temp = pthread_create(&transmitid,NULL,Uartthread,NULL);
		if(temp !=0)
		{
			printf("can't create Uartthread\n");  
    		return 1;  
		}
		temp = pthread_create(&rtcid,NULL,Rtcthread,NULL);
		if(temp !=0)
		{
			printf("can't create Rtcthread\n");  
    		return 1;  
		}

		
		while(1);
		}

	//
	exit(0);
}
