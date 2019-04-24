
#include     <stdio.h> 
#include     <stdlib.h>
#include     <stdint.h>
#include     <string.h>
#include     <unistd.h>
#include     <sys/wait.h>  

#include     <sys/types.h>
#include     <sys/stat.h>  
#include     <fcntl.h>  
#include     <termios.h>
#include     <errno.h> 
#include     <pthread.h> 
#include     <sys/ioctl.h> 
#include     "../include/serialuart.h"
#include     "../include/systemrtc.h"
#include     "../include/socket.h"



#define AirStationCMDLength       8


int speed_arr[] = {  B115200, B57600, B38400, B19200, B9600, B4800,
		    B2400, B1200};
int name_arr[] = {115200, 57600, 38400,  19200,  9600,  4800,  2400, 1200};




const uint8_t StartTH[AirStationCMDLength] = {0x03,0x03,0x00,0x00,0x00,0x02,0xC5,0xE9};             //温湿度
const uint8_t StartWS[AirStationCMDLength] = {0x01,0x03,0x00,0x00,0x00,0x01,0x84,0x0A};             //风速
const uint8_t StartWD[AirStationCMDLength] = {0x02,0x03,0x00,0x00,0x00,0x01,0x84,0x39};             //风向
//const uint8_t StartGas[GasCMDLength] = {0xFF,0xFF,0x01,0x01,0x05,0x01,0x00,0x6C,0x07,0x74,0xDD};    //SO2,CO,NO
const uint8_t StartPressure[AirStationCMDLength] = {0x03,0x03,0x00,0x02,0x00,0x01,0x24,0x28};       //气压
const uint8_t StartIndoor[AirStationCMDLength] = {0x03,0x03,0x00,0x03,0x00,0x03,0xF4,0x29};         //室内PM值
const uint8_t StartOutdoor[AirStationCMDLength] = {0x03,0x03,0x00,0x06,0x00,0x03,0xE4,0x28};        //室外PM值
const uint8_t StartGranules[AirStationCMDLength] = {0x03,0x03,0x00,0x09,0x00,0x06,0x14,0x28};       //颗粒物

uint8_t StoreCache[94];
uint8_t TfStoreBuf[45];


int fd=-1;
char buff[512];
int Acklength = 0;
AppStruct App;







void set_speed(int fd, int speed)
{
    int   i;
    int   status;
    struct termios   Opt;
    tcgetattr(fd, &Opt);
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
    {
   	if (speed == name_arr[i])
   	{
   	    tcflush(fd, TCIOFLUSH);
	    cfsetispeed(&Opt, speed_arr[i]);
	    cfsetospeed(&Opt, speed_arr[i]);
	    status = tcsetattr(fd, TCSANOW, &Opt);
	    if (status != 0)
		perror("tcsetattr fd1");
	    return;
     	}
	tcflush(fd,TCIOFLUSH);
    }
}

int set_Parity(int fd,int databits,int stopbits,int parity)
{
    struct termios options;
    if  ( tcgetattr( fd,&options)  !=  0)
    {
  	perror("SetupSerial 1");
  	return(FALSE);
    }
    options.c_cflag &= ~CSIZE;
    switch (databits)
    {
    case 7:
	options.c_cflag |= CS7;
	break;
    case 8:
	options.c_cflag |= CS8;
	break;
    default:
	fprintf(stderr,"Unsupported data size\n");
	return (FALSE);
    }
    switch (parity)
    {
    case 'n':
    case 'N':
	options.c_cflag &= ~PARENB;   
	options.c_iflag &= ~INPCK;   
	break;
    case 'o':
    case 'O':
	options.c_cflag |= (PARODD | PARENB); 
	options.c_iflag |= INPCK;           
	break;
    case 'e':
    case 'E':
	options.c_cflag |= PARENB;     
	options.c_cflag &= ~PARODD;
	options.c_iflag |= INPCK;     
	break;
    case 'S':
    case 's':  
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	break;
    default:
	fprintf(stderr,"Unsupported parity\n");
	return (FALSE);
    }
    switch (stopbits)
    {
    case 1:
	options.c_cflag &= ~CSTOPB;
	break;
    case 2:
	options.c_cflag |= CSTOPB;
	break;
    default:
	fprintf(stderr,"Unsupported stop bits\n");
	return (FALSE);
    }


	options.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
	options.c_oflag &= ~OPOST;
	options.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);

    /* Set input parity option */
    
    if (parity != 'n')
	options.c_iflag |= INPCK;
    options.c_cc[VTIME] = 150; // 15 seconds
    options.c_cc[VMIN] = 0;
    
    
		tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */
    if (tcsetattr(fd,TCSANOW,&options) != 0)
    {
	perror("SetupSerial 3");
	return (FALSE);
    }
    return (TRUE);
}

void receivethread(void)
{
  while(1)    
  {
	Acklength = read(fd,buff,100);
	usleep(100);

//	if(Acklength == 10){
//		if(SyncRTCRegulate(buff) == 0);
//		{
//			printf("Set system datatime done\n");
//		}
//		}
   } 
 
 return;
}


void *Uartthread(void *arg)
{
	int i = 0;
	static uint8_t n = 0;
	FILE *fp;
	char *backspace = "\r\n";
	static int scadaIndex = 0;
	static int PostIndex = 0;
	static char  st[2];
	pthread_t receiveid;
	pid_t status;  

	fd = open("/dev/ttymxc4", O_RDWR);
	if (fd < 0){
		printf("open device faild\n");
		exit(0);
	}
	set_speed(fd,9600); 
	set_Parity(fd,8,1,'N');
	usleep(1000);
	pthread_create(&receiveid,NULL,(void *)receivethread,NULL);

	while(1)
	{
		if(1){
			if(scadaIndex == 0){
				status = system("mount /dev/mmcblk0p1 /tfCard");

				if (-1 == status)  
					printf("mount tfCard error!\r\n");  
				else{
					printf("mount tfCard exit status value = [0x%x]\n", status);  

					if (WIFEXITED(status))  
					{  
						if (0 == WEXITSTATUS(status))  
							printf("mount tfCard ok!\r\n");  
						else  
							printf("run mount tfCard shell script fail, script exit code: %d\n", WEXITSTATUS(status));  
					}  
					else  
						printf("mount tfCard exit status = [%d]\n", WEXITSTATUS(status));  
					}
				}

			scadaIndex ++;
			usleep(200000);

			switch (scadaIndex)
				{
				case 2:
					write(fd,(uint8_t *)StartTH,AirStationCMDLength);
					StoreCache[n++] = 'F';
					StoreCache[n++] = 'F';
					break;
				case 3:
					if((Acklength == 9)&&(buff[0] == 0x03)&&(buff[2] == 0x04)){
						App.Data.AirStationData.Temp = (float)(buff[3]*256+buff[4] - 4000)/100;
						App.Data.AirStationData.Humidity = (float)(buff[5]*256+buff[6])/100;
			            for(i=3;i<7;i++){
			                sprintf(st,"%02X",buff[i]);
			                StoreCache[n++] = st[0];
			                StoreCache[n++] = st[1];
			            	}
						}
					
					for(i=0;i<Acklength;i++)
						buff[i] = 0x00;

					Acklength = 0;
					break;
				case 4:
					write(fd,(uint8_t *)StartPressure,AirStationCMDLength);
					break;
				case 5:
					if((Acklength == 9)&&(buff[0] == 0x03)&&(buff[2] == 0x04)){
						App.Data.AirStationData.Press = (float)(buff[4]*65536+buff[5]*256+buff[6])/10;
			            for(i=3;i<7;i++){
			                sprintf(st,"%02X",buff[i]);
			                StoreCache[n++] = st[0];
			                StoreCache[n++] = st[1];
			            	}
						}
					
					for(i=0;i<Acklength;i++)
						buff[i] = 0x00;

					Acklength = 0;
					break;
				case 6:
					write(fd,(uint8_t *)StartOutdoor,AirStationCMDLength);
					break;
				case 7:
					if((Acklength == 11)&&(buff[0] == 0x03)&&(buff[2] == 0x06)){
						App.Data.AirStationData.PM1_0 = buff[3]*256+buff[4];
						App.Data.AirStationData.PM25 = buff[5]*256+buff[6];
						App.Data.AirStationData.PM10  = buff[7]*256+buff[8];
			            for(i=3;i<9;i++){
			                sprintf(st,"%02X",buff[i]);
			                StoreCache[n++] = st[0];
			                StoreCache[n++] = st[1];
			            	}
						}
					
					for(i=0;i<Acklength;i++)
						buff[i] = 0x00;

					Acklength = 0;
					break;
				case 17:


						break;
				case 18:
					printf("Temp = %.2f C,Humi = %.2f%%,Press = %.1f pa,PM1.0 = %d ug/m3,PM2.5 = %d ug/m3,PM10 = %d ug/m3\n",
						App.Data.AirStationData.Temp,App.Data.AirStationData.Humidity,App.Data.AirStationData.Press,App.Data.AirStationData.PM1_0,App.Data.AirStationData.PM25,App.Data.AirStationData.PM10);
					for(i=0;i<512;i++)
						buff[i] = 0x00;
					
					Acklength = 0;
					break;	
				}

			if(scadaIndex == 20){
				for(i=0;i<6;i++){
					StoreCache[n++] = cTime[i];
					}
				if((fp=fopen(k30_storefile,"a+")) == NULL){
					printf("open file failed\n");
					exit(0);
					}
					else{
						fwrite(StoreCache,n,1,fp);
						fwrite(backspace,strlen(backspace),1,fp);
						fclose(fp);
						printf("write file done\n");
						}
				n = 0;
				
				scadaIndex = 1;
				}
			}		
	}
	close(fd);
}


