/*
********************************************************************************
*文件名     : data.h
********************************************************************************
*版本      作者             日期                说明
*V0.1    Guolz        2016/09/13        初始版本
********************************************************************************
*/


#ifndef __DATA_H
#define __DATA_H

#include     <stdint.h>


typedef struct
{
    float CODValue;
    float ORPValue;
    float TemValue;
    float DoPercent;
    float DOValue;
    float DORealValue;
    float NH4Value;
    float ZSValue;
    float PHValue;
    float DMValue;
    uint16_t LVValue;
}BlackRiverPlatform;

typedef struct
{
	float WindSpeed;
	float WindDirection;
	int WDir;
	float Temp;
	float Humidity;
	float Press;
	uint16_t PM25;
	uint16_t PM10;
	uint16_t PM1_0;
	float SulfurDioxide;
	float NitrogenDioxide;
	float CarbonMonoxide;
}AirStationPlatform;

typedef struct
{
    uint32_t Second;
    uint32_t Minute;
    uint32_t Hour;
    uint32_t Day;
    uint32_t Week;
    uint32_t Month;
    uint32_t Year;
}RtcStruct;

typedef struct
{
    float Temprature;
    float Humidity;
    float Press;
}BMEStruct;


typedef struct
{
    uint32_t DeviceID;
    uint32_t Version;
    uint32_t PowerQuanity;
    uint8_t PowerDataLen;
    uint16_t LoraDeveui;
    uint8_t DevType;
    uint8_t WirelessType;
}Terminal;


typedef struct
{
    char Voltage;
    BlackRiverPlatform  BlackRiverData;
    AirStationPlatform  AirStationData;
// 以下为系统保留数据，系统层直接使用    
    
    BMEStruct Bme;
    RtcStruct Rtc;
    
    Terminal TerminalInfoData;

    uint8_t SensorPower;
    uint32_t SerialNumber;              // SN
    uint32_t Date;
    uint8_t Month;
    uint8_t Day;
}DataStruct;

typedef struct
{
    DataStruct Data;
}AppStruct;







#endif /*__Data_H*/

