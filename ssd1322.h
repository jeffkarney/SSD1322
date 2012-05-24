/*
ssd1322.h - Library for SSD1322 based OLED's
Created by Jeff Karney (JK Devices) 2012
*/
#ifndef SSD1322_h
#define SSD1322_h

#include "Arduino.h"

#define	Shift		0x1C
#define Max_Column	0x3F			
#define Max_Row		0x3F
#define	Brightness	0x0F

class SSD1322{
	public:
		SSD1322(int sclk, int, sdin, int cs, int res);
		void write();
		
	private:
		void initialize();
		
		void Write_Command();
		void Write_Data();
		
		void Set_Command_Lock(unsigned char d);
		void Set_Display_On_Off(unsigned char d);
		void Set_Column_Address(unsigned char a, unsigned char b);
		void Set_Row_Address(unsigned char a, unsigned char b);
		void Set_Display_Clock(unsigned char d);
		void Set_Multiplex_Ratio(unsigned char d);
		void Set_Display_Offset(unsigned char d);
		void Set_Start_Line(unsigned char d);
		void Set_Remap_Format(unsigned char d); 
		void Set_GPIO(unsigned char d);
		void Set_Function_Selection(unsigned char d);
		void Set_Display_Enhancement_A(unsigned char a, unsigned char b);
		void Set_Display_Enhancement_B(unsigned char d);
		void Set_Contrast_Current(unsigned char d);
		void Set_Master_Current(unsigned char d);
		void Set_Linear_Gray_Scale_Table();
		void Set_Phase_Length(unsigned char d);
		void Set_Precharge_Voltage(unsigned char d);
		void Set_Precharge_Period(unsigned char d);
		void Set_VCOMH(unsigned char d);
		void Set_Display_Mode(unsigned char d);
		void Set_Partial_Display(unsigned char a, unsigned char b, unsigned char c);
		void Set_Display_On_Off(unsigned char d);
		
		unsigned char num[2] = {0x00, 0x00};
		unsigned char thechar[5] = {0x00,0x00,0x00,0x00,0x00};
		
};
#endif