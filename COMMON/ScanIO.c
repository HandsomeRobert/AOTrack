/**************************************************************************************************
*************************本文件用于将STM32配置的IO信息写入到XML文档********************************
***************************************************************************************************/

#include "ScanIO.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* NumberToChar(char** p, int number)
{
	sprintf(*p, "%d", number);
	return *p;	
}


void WriteDownIOInfo(void)
{
	char* IOInfo;
	FIL* fp;
	UINT bw;
	int res = 0;
	char* TempChar;
	
	
	
	ezxml_t root = ezxml_new("IOCollectData");
	
	ezxml_t Child_0 = ezxml_add_child(root, "Client", 0);
	ezxml_t Child_1 = ezxml_add_child(root, "Synchron", 0);
	
	ezxml_t Child_0_0;
	ezxml_t Child_0_1;
	ezxml_t Child_0_2;
	
	ezxml_t Child_1_0; 
	ezxml_t Child_1_1; 
	ezxml_t Child_1_2; 
	ezxml_t Child_1_3; 
	ezxml_t Child_1_4; 
	
	ezxml_t Child_1_4_0;
	ezxml_t Child_1_4_1;
	ezxml_t Child_1_4_2;
	ezxml_t Child_1_4_3;
	ezxml_t Child_1_4_4;
	ezxml_t Child_1_4_5;
	ezxml_t Child_1_4_6;
	ezxml_t Child_1_4_7;
	ezxml_t Child_1_4_8;
	ezxml_t Child_1_4_9;
	ezxml_t Child_1_4_10;
	ezxml_t Child_1_4_11;
	ezxml_t Child_1_4_12;
	ezxml_t Child_1_4_13;
	ezxml_t Child_1_4_14;
	ezxml_t Child_1_4_15;
	ezxml_t Child_1_4_16;
	ezxml_t Child_1_4_17;
	ezxml_t Child_1_4_18;
	ezxml_t Child_1_4_19;
	ezxml_t Child_1_4_20;
	ezxml_t Child_1_4_21;
	ezxml_t Child_1_4_22;
	ezxml_t Child_1_4_23;
	ezxml_t Child_1_4_24;
	
	ezxml_t Child_1_4_0_0;
	ezxml_t Child_1_4_0_1;
	ezxml_t Child_1_4_0_2;
	ezxml_t Child_1_4_0_3;
	ezxml_t Child_1_4_0_4;
	 ezxml_t Child_1_4_1_0;
	ezxml_t Child_1_4_1_1;
	ezxml_t Child_1_4_1_2;
	ezxml_t Child_1_4_1_3;
	ezxml_t Child_1_4_1_4;
	 ezxml_t Child_1_4_2_0;
	ezxml_t Child_1_4_2_1;
	ezxml_t Child_1_4_2_2;
	ezxml_t Child_1_4_2_3;
	ezxml_t Child_1_4_2_4;
	 ezxml_t Child_1_4_3_0;
	ezxml_t Child_1_4_3_1;
	ezxml_t Child_1_4_3_2;
	ezxml_t Child_1_4_3_3;
	ezxml_t Child_1_4_3_4;
		ezxml_t Child_1_4_4_0;
	ezxml_t Child_1_4_4_1;
	ezxml_t Child_1_4_4_2;
	ezxml_t Child_1_4_4_3;
	ezxml_t Child_1_4_4_4;
		ezxml_t Child_1_4_5_0;
	ezxml_t Child_1_4_5_1;
	ezxml_t Child_1_4_5_2;
	ezxml_t Child_1_4_5_3;
	ezxml_t Child_1_4_5_4;
		ezxml_t Child_1_4_6_0;
	ezxml_t Child_1_4_6_1;
	ezxml_t Child_1_4_6_2;
	ezxml_t Child_1_4_6_3;
	ezxml_t Child_1_4_6_4;
		ezxml_t Child_1_4_7_0;
	ezxml_t Child_1_4_7_1;
	ezxml_t Child_1_4_7_2;
	ezxml_t Child_1_4_7_3;
	ezxml_t Child_1_4_7_4;
		ezxml_t Child_1_4_8_0;
	ezxml_t Child_1_4_8_1;
	ezxml_t Child_1_4_8_2;
	ezxml_t Child_1_4_8_3;
	ezxml_t Child_1_4_8_4;
		ezxml_t Child_1_4_9_0;
	ezxml_t Child_1_4_9_1;
	ezxml_t Child_1_4_9_2;
	ezxml_t Child_1_4_9_3;
	ezxml_t Child_1_4_9_4;
		ezxml_t Child_1_4_10_0;
	ezxml_t Child_1_4_10_1;
	ezxml_t Child_1_4_10_2;
	ezxml_t Child_1_4_10_3;
	ezxml_t Child_1_4_10_4;
		ezxml_t Child_1_4_11_0;
	ezxml_t Child_1_4_11_1;
	ezxml_t Child_1_4_11_2;
	ezxml_t Child_1_4_11_3;
	ezxml_t Child_1_4_11_4;
		ezxml_t Child_1_4_12_0;
	ezxml_t Child_1_4_12_1;
	ezxml_t Child_1_4_12_2;
	ezxml_t Child_1_4_12_3;
	ezxml_t Child_1_4_12_4;
		ezxml_t Child_1_4_13_0;
	ezxml_t Child_1_4_13_1;
	ezxml_t Child_1_4_13_2;
	ezxml_t Child_1_4_13_3;
	ezxml_t Child_1_4_13_4;
		ezxml_t Child_1_4_14_0;
	ezxml_t Child_1_4_14_1;
	ezxml_t Child_1_4_14_2;
	ezxml_t Child_1_4_14_3;
	ezxml_t Child_1_4_14_4;
		ezxml_t Child_1_4_15_0;
	ezxml_t Child_1_4_15_1;
	ezxml_t Child_1_4_15_2;
	ezxml_t Child_1_4_15_3;
	ezxml_t Child_1_4_15_4;
		ezxml_t Child_1_4_16_0;
	ezxml_t Child_1_4_16_1;
	ezxml_t Child_1_4_16_2;
	ezxml_t Child_1_4_16_3;
	ezxml_t Child_1_4_16_4;
		ezxml_t Child_1_4_17_0;
	ezxml_t Child_1_4_17_1;
	ezxml_t Child_1_4_17_2;
	ezxml_t Child_1_4_17_3;
	ezxml_t Child_1_4_17_4;
		ezxml_t Child_1_4_18_0;
	ezxml_t Child_1_4_18_1;
	ezxml_t Child_1_4_18_2;
	ezxml_t Child_1_4_18_3;
	ezxml_t Child_1_4_18_4;
		ezxml_t Child_1_4_19_0;
	ezxml_t Child_1_4_19_1;
	ezxml_t Child_1_4_19_2;
	ezxml_t Child_1_4_19_3;
	ezxml_t Child_1_4_19_4;
		ezxml_t Child_1_4_20_0;
	ezxml_t Child_1_4_20_1;
	ezxml_t Child_1_4_20_2;
	ezxml_t Child_1_4_20_3;
	ezxml_t Child_1_4_20_4;
		ezxml_t Child_1_4_21_0;
	ezxml_t Child_1_4_21_1;
	ezxml_t Child_1_4_21_2;
	ezxml_t Child_1_4_21_3;
	ezxml_t Child_1_4_21_4;
		ezxml_t Child_1_4_22_0;
	ezxml_t Child_1_4_22_1;
	ezxml_t Child_1_4_22_2;
	ezxml_t Child_1_4_22_3;
	ezxml_t Child_1_4_22_4;
		ezxml_t Child_1_4_23_0;
	ezxml_t Child_1_4_23_1;
	ezxml_t Child_1_4_23_2;
	ezxml_t Child_1_4_23_3;
	ezxml_t Child_1_4_23_4;
		ezxml_t Child_1_4_24_0;
	ezxml_t Child_1_4_24_1;
	ezxml_t Child_1_4_24_2;
	ezxml_t Child_1_4_24_3;
	ezxml_t Child_1_4_24_4;
	
	
	Child_0_0 = ezxml_add_child(Child_0, "ClientIP", 0);
	Child_0_1 = ezxml_add_child(Child_0, "PortSend", 0);
  Child_0_2 = ezxml_add_child(Child_0, "PortReceive", 0);
	ezxml_set_txt(Child_0_0, "192.168.66.10");
	ezxml_set_txt(Child_0_1, "5020");
	ezxml_set_txt(Child_0_2, "5021");

	Child_1_0 = ezxml_add_child(Child_1, "MaxInterpolation", 0);
	Child_1_1 = ezxml_add_child(Child_1, "BufferBits", 0);
	Child_1_2 = ezxml_add_child(Child_1, "CyclesPerSampling", 0);
	Child_1_3 = ezxml_add_child(Child_1, "CyclesPerSend", 0);
	Child_1_4 = ezxml_add_child(Child_1, "ListChannel", 0);
	ezxml_set_txt(Child_1_0, "10");
	ezxml_set_txt(Child_1_1, "12");
	ezxml_set_txt(Child_1_2, "1");
	ezxml_set_txt(Child_1_3, "1");
	
	Child_1_4_0  = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_1  = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_2  = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_3  = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_4  = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_5  = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_6  = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_7  = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_8  = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_9  = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_10 = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_11 = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_12 = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_13 = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_14 = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_15 = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_16 = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_17 = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_18 = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_19 = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_20 = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_21 = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_22 = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_23 = ezxml_add_child(Child_1_4, "IOChannel", 0);
	Child_1_4_24 = ezxml_add_child(Child_1_4, "IOChannel", 0);
	
	
	//0
	Child_1_4_0_0 = ezxml_add_child(Child_1_4_0, "ConfigurationID", 0);
	Child_1_4_0_1 = ezxml_add_child(Child_1_4_0, "ChannelID", 0);
	Child_1_4_0_2 = ezxml_add_child(Child_1_4_0, "HardwareID", 0);
	Child_1_4_0_3 = ezxml_add_child(Child_1_4_0, "ChannelName", 0);
	Child_1_4_0_4 = ezxml_add_child(Child_1_4_0, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_0_0, "111111");
	ezxml_set_txt(Child_1_4_0_1, "1");
	ezxml_set_txt(Child_1_4_0_2, NumberToChar(&TempChar, LightSignal_1));
	ezxml_set_txt(Child_1_4_0_3, "LightSignal_1");
	ezxml_set_txt(Child_1_4_0_4, "LightSignalIn");
	//1
	Child_1_4_1_0 = ezxml_add_child(Child_1_4_1, "ConfigurationID", 0);
	Child_1_4_1_1 = ezxml_add_child(Child_1_4_1, "ChannelID", 0);
	Child_1_4_1_2 = ezxml_add_child(Child_1_4_1, "HardwareID", 0);
	Child_1_4_1_3 = ezxml_add_child(Child_1_4_1, "ChannelName", 0);
	Child_1_4_1_4 = ezxml_add_child(Child_1_4_1, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_1_0, "111112");
	ezxml_set_txt(Child_1_4_1_1, "2");
	ezxml_set_txt(Child_1_4_1_2, NumberToChar(&TempChar, LightSignal_2));
	ezxml_set_txt(Child_1_4_1_3, "LightSignal_2");
	ezxml_set_txt(Child_1_4_1_4, "LightSignalIn");
	//2
	Child_1_4_2_0 = ezxml_add_child(Child_1_4_2, "ConfigurationID", 0);
	Child_1_4_2_1 = ezxml_add_child(Child_1_4_2, "ChannelID", 0);
	Child_1_4_2_2 = ezxml_add_child(Child_1_4_2, "HardwareID", 0);
	Child_1_4_2_3 = ezxml_add_child(Child_1_4_2, "ChannelName", 0);
	Child_1_4_2_4 = ezxml_add_child(Child_1_4_2, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_2_0, "111113");
	ezxml_set_txt(Child_1_4_2_1, "3");
	ezxml_set_txt(Child_1_4_2_2, NumberToChar(&TempChar, LightSignal_3));
	ezxml_set_txt(Child_1_4_2_3, "LightSignal_3");
	ezxml_set_txt(Child_1_4_2_4, "LightSignalIn");
	//3
	Child_1_4_3_0 = ezxml_add_child(Child_1_4_3, "ConfigurationID", 0);
	Child_1_4_3_1 = ezxml_add_child(Child_1_4_3, "ChannelID", 0);
	Child_1_4_3_2 = ezxml_add_child(Child_1_4_3, "HardwareID", 0);
	Child_1_4_3_3 = ezxml_add_child(Child_1_4_3, "ChannelName", 0);
	Child_1_4_3_4 = ezxml_add_child(Child_1_4_3, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_3_0, "111114");
	ezxml_set_txt(Child_1_4_3_1, "4");
	ezxml_set_txt(Child_1_4_3_2, NumberToChar(&TempChar, LightSignal_4));
	ezxml_set_txt(Child_1_4_3_3, "LightSignal_4");
	ezxml_set_txt(Child_1_4_3_4, "LightSignalIn");
	//4
	Child_1_4_4_0 = ezxml_add_child(Child_1_4_4, "ConfigurationID", 0);
	Child_1_4_4_1 = ezxml_add_child(Child_1_4_4, "ChannelID", 0);
	Child_1_4_4_2 = ezxml_add_child(Child_1_4_4, "HardwareID", 0);
	Child_1_4_4_3 = ezxml_add_child(Child_1_4_4, "ChannelName", 0);
	Child_1_4_4_4 = ezxml_add_child(Child_1_4_4, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_4_0, "111115");
	ezxml_set_txt(Child_1_4_4_1, "5");
	ezxml_set_txt(Child_1_4_4_2, NumberToChar(&TempChar, LightSignal_5));
	ezxml_set_txt(Child_1_4_4_3, "LightSignal_5");
	ezxml_set_txt(Child_1_4_4_4, "LightSignalIn");
	//5
	Child_1_4_5_0 = ezxml_add_child(Child_1_4_5, "ConfigurationID", 0);
	Child_1_4_5_1 = ezxml_add_child(Child_1_4_5, "ChannelID", 0);
	Child_1_4_5_2 = ezxml_add_child(Child_1_4_5, "HardwareID", 0);
	Child_1_4_5_3 = ezxml_add_child(Child_1_4_5, "ChannelName", 0);
	Child_1_4_5_4 = ezxml_add_child(Child_1_4_5, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_5_0, "111116");
	ezxml_set_txt(Child_1_4_5_1, "6");
	ezxml_set_txt(Child_1_4_5_2, NumberToChar(&TempChar, LightSignal_6));
	ezxml_set_txt(Child_1_4_5_3, "LightSignal_6");
	ezxml_set_txt(Child_1_4_5_4, "LightSignalIn");
	//6
	Child_1_4_6_0 = ezxml_add_child(Child_1_4_6, "ConfigurationID", 0);
	Child_1_4_6_1 = ezxml_add_child(Child_1_4_6, "ChannelID", 0);
	Child_1_4_6_2 = ezxml_add_child(Child_1_4_6, "HardwareID", 0);
	Child_1_4_6_3 = ezxml_add_child(Child_1_4_6, "ChannelName", 0);
	Child_1_4_6_4 = ezxml_add_child(Child_1_4_6, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_6_0, "111117");
	ezxml_set_txt(Child_1_4_6_1, "7");
	ezxml_set_txt(Child_1_4_6_2, NumberToChar(&TempChar, LightSignal_7));
	ezxml_set_txt(Child_1_4_6_3, "LightSignal_7");
	ezxml_set_txt(Child_1_4_6_4, "LightSignalIn");
	//7
	Child_1_4_7_0 = ezxml_add_child(Child_1_4_7, "ConfigurationID", 0);
	Child_1_4_7_1 = ezxml_add_child(Child_1_4_7, "ChannelID", 0);
	Child_1_4_7_2 = ezxml_add_child(Child_1_4_7, "HardwareID", 0);
	Child_1_4_7_3 = ezxml_add_child(Child_1_4_7, "ChannelName", 0);
	Child_1_4_7_4 = ezxml_add_child(Child_1_4_7, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_7_0, "111118");
	ezxml_set_txt(Child_1_4_7_1, "8");
	ezxml_set_txt(Child_1_4_7_2, NumberToChar(&TempChar, Encoder_1));
	ezxml_set_txt(Child_1_4_7_3, "Encoder_1");
	ezxml_set_txt(Child_1_4_7_4, "Encoder32");
	//8
	Child_1_4_8_0 = ezxml_add_child(Child_1_4_8, "ConfigurationID", 0);
	Child_1_4_8_1 = ezxml_add_child(Child_1_4_8, "ChannelID", 0);
	Child_1_4_8_2 = ezxml_add_child(Child_1_4_8, "HardwareID", 0);
	Child_1_4_8_3 = ezxml_add_child(Child_1_4_8, "ChannelName", 0);
	Child_1_4_8_4 = ezxml_add_child(Child_1_4_8, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_8_0, "111119");
	ezxml_set_txt(Child_1_4_8_1, "9");
	ezxml_set_txt(Child_1_4_8_2, NumberToChar(&TempChar, Encoder_2));
	ezxml_set_txt(Child_1_4_8_3, "Encoder_2");
	ezxml_set_txt(Child_1_4_8_4, "Encoder32");
	//9
	Child_1_4_9_0 = ezxml_add_child(Child_1_4_9, "ConfigurationID", 0);
	Child_1_4_9_1 = ezxml_add_child(Child_1_4_9, "ChannelID", 0);
	Child_1_4_9_2 = ezxml_add_child(Child_1_4_9, "HardwareID", 0);
	Child_1_4_9_3 = ezxml_add_child(Child_1_4_9, "ChannelName", 0);
	Child_1_4_9_4 = ezxml_add_child(Child_1_4_9, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_9_0, "111120");
	ezxml_set_txt(Child_1_4_9_1, "10");
	ezxml_set_txt(Child_1_4_9_2, NumberToChar(&TempChar, DigitalOutput_1));
	ezxml_set_txt(Child_1_4_9_3, "DigitalOutput_1");
	ezxml_set_txt(Child_1_4_9_4, "DigitalOutput");
	//10
	Child_1_4_10_0 = ezxml_add_child(Child_1_4_10, "ConfigurationID", 0);
	Child_1_4_10_1 = ezxml_add_child(Child_1_4_10, "ChannelID", 0);
	Child_1_4_10_2 = ezxml_add_child(Child_1_4_10, "HardwareID", 0);
	Child_1_4_10_3 = ezxml_add_child(Child_1_4_10, "ChannelName", 0);
	Child_1_4_10_4 = ezxml_add_child(Child_1_4_10, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_10_0, "111121");
	ezxml_set_txt(Child_1_4_10_1, "11");
	ezxml_set_txt(Child_1_4_10_2, NumberToChar(&TempChar, DigitalOutput_2));
	ezxml_set_txt(Child_1_4_10_3, "DigitalOutput_2");
	ezxml_set_txt(Child_1_4_10_4, "DigitalOutput");
	//11
	Child_1_4_11_0 = ezxml_add_child(Child_1_4_11, "ConfigurationID", 0);
	Child_1_4_11_1 = ezxml_add_child(Child_1_4_11, "ChannelID", 0);
	Child_1_4_11_2 = ezxml_add_child(Child_1_4_11, "HardwareID", 0);
	Child_1_4_11_3 = ezxml_add_child(Child_1_4_11, "ChannelName", 0);
	Child_1_4_11_4 = ezxml_add_child(Child_1_4_11, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_11_0, "111122");
	ezxml_set_txt(Child_1_4_11_1, "12");
	ezxml_set_txt(Child_1_4_11_2, NumberToChar(&TempChar, DigitalOutput_3));
	ezxml_set_txt(Child_1_4_11_3, "DigitalOutput_3");
	ezxml_set_txt(Child_1_4_11_4, "DigitalOutput");
	//12
	Child_1_4_12_0 = ezxml_add_child(Child_1_4_12, "ConfigurationID", 0);
	Child_1_4_12_1 = ezxml_add_child(Child_1_4_12, "ChannelID", 0);
	Child_1_4_12_2 = ezxml_add_child(Child_1_4_12, "HardwareID", 0);
	Child_1_4_12_3 = ezxml_add_child(Child_1_4_12, "ChannelName", 0);
	Child_1_4_12_4 = ezxml_add_child(Child_1_4_12, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_12_0, "111123");
	ezxml_set_txt(Child_1_4_12_1, "13");
	ezxml_set_txt(Child_1_4_12_2, NumberToChar(&TempChar, DigitalOutput_4));
	ezxml_set_txt(Child_1_4_12_3, "DigitalOutput_4");
	ezxml_set_txt(Child_1_4_12_4, "DigitalOutput");
	//13
	Child_1_4_13_0 = ezxml_add_child(Child_1_4_13, "ConfigurationID", 0);
	Child_1_4_13_1 = ezxml_add_child(Child_1_4_13, "ChannelID", 0);
	Child_1_4_13_2 = ezxml_add_child(Child_1_4_13, "HardwareID", 0);
	Child_1_4_13_3 = ezxml_add_child(Child_1_4_13, "ChannelName", 0);
	Child_1_4_13_4 = ezxml_add_child(Child_1_4_13, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_13_0, "111124");
	ezxml_set_txt(Child_1_4_13_1, "14");
	ezxml_set_txt(Child_1_4_13_2, NumberToChar(&TempChar, DigitalOutput_5));
	ezxml_set_txt(Child_1_4_13_3, "DigitalOutput_5");
	ezxml_set_txt(Child_1_4_13_4, "DigitalOutput");
	//14	
	Child_1_4_14_0 = ezxml_add_child(Child_1_4_14, "ConfigurationID", 0);
	Child_1_4_14_1 = ezxml_add_child(Child_1_4_14, "ChannelID", 0);
	Child_1_4_14_2 = ezxml_add_child(Child_1_4_14, "HardwareID", 0);
	Child_1_4_14_3 = ezxml_add_child(Child_1_4_14, "ChannelName", 0);
	Child_1_4_14_4 = ezxml_add_child(Child_1_4_14, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_14_0, "111125");
	ezxml_set_txt(Child_1_4_14_1, "15");
	ezxml_set_txt(Child_1_4_14_2, NumberToChar(&TempChar, DigitalOutput_6));
	ezxml_set_txt(Child_1_4_14_3, "DigitalOutput_6");
	ezxml_set_txt(Child_1_4_14_4, "DigitalOutput");
	//15
	Child_1_4_15_0 = ezxml_add_child(Child_1_4_15, "ConfigurationID", 0);
	Child_1_4_15_1 = ezxml_add_child(Child_1_4_15, "ChannelID", 0);
	Child_1_4_15_2 = ezxml_add_child(Child_1_4_15, "HardwareID", 0);
	Child_1_4_15_3 = ezxml_add_child(Child_1_4_15, "ChannelName", 0);
	Child_1_4_15_4 = ezxml_add_child(Child_1_4_15, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_15_0, "111126");
	ezxml_set_txt(Child_1_4_15_1, "16");
	ezxml_set_txt(Child_1_4_15_2, NumberToChar(&TempChar, DigitalOutput_7));
	ezxml_set_txt(Child_1_4_15_3, "DigitalOutput_7");
	ezxml_set_txt(Child_1_4_15_4, "DigitalOutput");
	//16	
	Child_1_4_16_0 = ezxml_add_child(Child_1_4_16, "ConfigurationID", 0);
	Child_1_4_16_1 = ezxml_add_child(Child_1_4_16, "ChannelID", 0);
	Child_1_4_16_2 = ezxml_add_child(Child_1_4_16, "HardwareID", 0);
	Child_1_4_16_3 = ezxml_add_child(Child_1_4_16, "ChannelName", 0);
	Child_1_4_16_4 = ezxml_add_child(Child_1_4_16, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_16_0, "111127");
	ezxml_set_txt(Child_1_4_16_1, "17");
	ezxml_set_txt(Child_1_4_16_2, NumberToChar(&TempChar, DigitalOutput_8));
	ezxml_set_txt(Child_1_4_16_3, "DigitalOutput_8");
	ezxml_set_txt(Child_1_4_16_4, "DigitalOutput");
	//17
	Child_1_4_17_0 = ezxml_add_child(Child_1_4_17, "ConfigurationID", 0);
	Child_1_4_17_1 = ezxml_add_child(Child_1_4_17, "ChannelID", 0);
	Child_1_4_17_2 = ezxml_add_child(Child_1_4_17, "HardwareID", 0);
	Child_1_4_17_3 = ezxml_add_child(Child_1_4_17, "ChannelName", 0);
	Child_1_4_17_4 = ezxml_add_child(Child_1_4_17, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_17_0, "111128");
	ezxml_set_txt(Child_1_4_17_1, "18");
	ezxml_set_txt(Child_1_4_17_2, NumberToChar(&TempChar, DigitalInput_1));
	ezxml_set_txt(Child_1_4_17_3, "DigitalInput_1");
	ezxml_set_txt(Child_1_4_17_4, "DigitalInput");
	//18	
	Child_1_4_18_0 = ezxml_add_child(Child_1_4_18, "ConfigurationID", 0);
	Child_1_4_18_1 = ezxml_add_child(Child_1_4_18, "ChannelID", 0);
	Child_1_4_18_2 = ezxml_add_child(Child_1_4_18, "HardwareID", 0);
	Child_1_4_18_3 = ezxml_add_child(Child_1_4_18, "ChannelName", 0);
	Child_1_4_18_4 = ezxml_add_child(Child_1_4_18, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_18_0, "111129");
	ezxml_set_txt(Child_1_4_18_1, "19");
	ezxml_set_txt(Child_1_4_18_2, NumberToChar(&TempChar, DigitalInput_2));
	ezxml_set_txt(Child_1_4_18_3, "DigitalInput_2");
	ezxml_set_txt(Child_1_4_18_4, "DigitalInput");
	//19
	Child_1_4_19_0 = ezxml_add_child(Child_1_4_19, "ConfigurationID", 0);
	Child_1_4_19_1 = ezxml_add_child(Child_1_4_19, "ChannelID", 0);
	Child_1_4_19_2 = ezxml_add_child(Child_1_4_19, "HardwareID", 0);
	Child_1_4_19_3 = ezxml_add_child(Child_1_4_19, "ChannelName", 0);
	Child_1_4_19_4 = ezxml_add_child(Child_1_4_19, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_19_0, "111130");
	ezxml_set_txt(Child_1_4_19_1, "20");
	ezxml_set_txt(Child_1_4_19_2, NumberToChar(&TempChar, DigitalInput_3));
	ezxml_set_txt(Child_1_4_19_3, "DigitalInput_3");
	ezxml_set_txt(Child_1_4_19_4, "DigitalInput");
	//20	
	Child_1_4_20_0 = ezxml_add_child(Child_1_4_20, "ConfigurationID", 0);
	Child_1_4_20_1 = ezxml_add_child(Child_1_4_20, "ChannelID", 0);
	Child_1_4_20_2 = ezxml_add_child(Child_1_4_20, "HardwareID", 0);
	Child_1_4_20_3 = ezxml_add_child(Child_1_4_20, "ChannelName", 0);
	Child_1_4_20_4 = ezxml_add_child(Child_1_4_20, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_20_0, "111131");
	ezxml_set_txt(Child_1_4_20_1, "21");
	ezxml_set_txt(Child_1_4_20_2, NumberToChar(&TempChar, DigitalInput_4));
	ezxml_set_txt(Child_1_4_20_3, "DigitalInput_4");
	ezxml_set_txt(Child_1_4_20_4, "DigitalInput");
	//21
	Child_1_4_21_0 = ezxml_add_child(Child_1_4_21, "ConfigurationID", 0);
	Child_1_4_21_1 = ezxml_add_child(Child_1_4_21, "ChannelID", 0);
	Child_1_4_21_2 = ezxml_add_child(Child_1_4_21, "HardwareID", 0);
	Child_1_4_21_3 = ezxml_add_child(Child_1_4_21, "ChannelName", 0);
	Child_1_4_21_4 = ezxml_add_child(Child_1_4_21, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_21_0, "111132");
	ezxml_set_txt(Child_1_4_21_1, "22");
	ezxml_set_txt(Child_1_4_21_2, NumberToChar(&TempChar, DigitalInput_5));
	ezxml_set_txt(Child_1_4_21_3, "DigitalInput_5");
	ezxml_set_txt(Child_1_4_21_4, "DigitalInput");
	//22	
	Child_1_4_22_0 = ezxml_add_child(Child_1_4_22, "ConfigurationID", 0);
	Child_1_4_22_1 = ezxml_add_child(Child_1_4_22, "ChannelID", 0);
	Child_1_4_22_2 = ezxml_add_child(Child_1_4_22, "HardwareID", 0);
	Child_1_4_22_3 = ezxml_add_child(Child_1_4_22, "ChannelName", 0);
	Child_1_4_22_4 = ezxml_add_child(Child_1_4_22, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_22_0, "111133");
	ezxml_set_txt(Child_1_4_22_1, "23");
	ezxml_set_txt(Child_1_4_22_2, NumberToChar(&TempChar, DigitalInput_6));
	ezxml_set_txt(Child_1_4_22_3, "DigitalInput_6");
	ezxml_set_txt(Child_1_4_22_4, "DigitalInput");
	//23
	Child_1_4_23_0 = ezxml_add_child(Child_1_4_23, "ConfigurationID", 0);
	Child_1_4_23_1 = ezxml_add_child(Child_1_4_23, "ChannelID", 0);
	Child_1_4_23_2 = ezxml_add_child(Child_1_4_23, "HardwareID", 0);
	Child_1_4_23_3 = ezxml_add_child(Child_1_4_23, "ChannelName", 0);
	Child_1_4_23_4 = ezxml_add_child(Child_1_4_23, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_23_0, "111134");
	ezxml_set_txt(Child_1_4_23_1, "24");
	ezxml_set_txt(Child_1_4_23_2, NumberToChar(&TempChar, DigitalInput_7));
	ezxml_set_txt(Child_1_4_23_3, "DigitalInput_7");
	ezxml_set_txt(Child_1_4_23_4, "DigitalInput");
	//24
	Child_1_4_24_0 = ezxml_add_child(Child_1_4_24, "ConfigurationID", 0);
	Child_1_4_24_1 = ezxml_add_child(Child_1_4_24, "ChannelID", 0);
	Child_1_4_24_2 = ezxml_add_child(Child_1_4_24, "HardwareID", 0);
	Child_1_4_24_3 = ezxml_add_child(Child_1_4_24, "ChannelName", 0);
	Child_1_4_24_4 = ezxml_add_child(Child_1_4_24, "ChannelType", 0);
	ezxml_set_txt(Child_1_4_24_0, "111135");
	ezxml_set_txt(Child_1_4_24_1, "25");
	ezxml_set_txt(Child_1_4_24_2, NumberToChar(&TempChar, DigitalInput_8));
	ezxml_set_txt(Child_1_4_24_3, "DigitalInput_8");
	ezxml_set_txt(Child_1_4_24_4, "DigitalInput");
	
	
	fp = mymalloc(SRAMEX, sizeof(FIL));
	printf("Sizeof FIL==>%d  \n", sizeof(FIL));
	
	res = f_open(fp, "IO.xml", FA_CREATE_ALWAYS);	//覆盖的形式
	printf("Create IO.xml Result[%d] \n", res);
	
	res = f_open(fp, "IO.xml", FA_WRITE);	//写形式打开
	printf("Open IO.xml Result[%d] \n", res);
	
	IOInfo = mymalloc(SRAMEX, 10240);
	IOInfo = ezxml_toxml(root);										
//	printf("IOInfo==>%s \n", IOInfo);
	
	printf("IOInfo string Length ==>%d \n", strlen(IOInfo));
	res = f_write(fp, IOInfo, strlen(IOInfo), &bw);
	if(res !=FR_OK)
	{
		printf("Document Write Failed ErrorInfo:[%d]\n", res);
	}
	
  myfree(SRAMEX, IOInfo);
	
//释放创建的对象   不知为何ezxml_t
	ezxml_free(root);
	
	res = f_close(fp);
	printf("Close IO.xml Result[%d] \n", res);
	myfree(SRAMEX, fp);
}

