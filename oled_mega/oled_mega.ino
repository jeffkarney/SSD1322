#include <digitalWriteFast.h>
#include <avr/pgmspace.h>
#include "glcdfont.c";

#include "Arduino.h"
#include <EEPROM.h>
#include <string.h>


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Pin Definition
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#define OLED_RES      9 // Reset
#define OLED_ERD 5 //E/RD#  //6800 = E  //8080 = RD
#define OLED_WR  2
#define OLED_DC  3
#define OLED_CS  8


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Global Variables
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#define	Shift		0x1C
#define Max_Column	0x3F			// 256/4-1
#define Max_Row		0x3F			// 64-1
#define	Brightness	0x0F


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//  Patterns

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
unsigned char num[2] = {0x00, 0x00};
unsigned char thechar[5] = {0x00,0x00,0x00,0x00,0x00};


byte command_or_data = 0;  //0 command, 1 data



unsigned char space[] = " ";
unsigned char aster[] = "*";


unsigned char single[2] = {32, 0};


void setup(){
    
    pinModeFast(OLED_ERD, OUTPUT);
    pinModeFast(OLED_WR, OUTPUT);
    pinModeFast(OLED_DC, OUTPUT);
    pinModeFast(OLED_CS, OUTPUT);
    pinMode(OLED_RES, OUTPUT);
    
    DDRK = B11111111;
    
    digitalWriteFast(OLED_ERD,HIGH);
    digitalWriteFast(OLED_WR,LOW);
    digitalWriteFast(OLED_DC,LOW);
    digitalWriteFast(OLED_CS,HIGH);
    
    OLED_Init(); 

}


void loop()
{
  
  Grayscale();
  delay(3000);
  clearScreen();
  Show_Pattern(&Logo[0][0],0x08,0x37,0x00,Max_Row);
  delay(3000);
  clearScreen();
  Draw_Rectangle(0xFF,0x01,0x00,Max_Column,0x00,Max_Row);
  delay(3000);
}

void clearScreen(){
  Fill_RAM(0x00);
}



/**************************************/
/* START OLED STUFF
/**************************************/




//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Read/Write Sequence
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//dc = 3
//erd = 5
//cs = 8

void Write_Command(unsigned char Data){
  if(command_or_data == 1){
    digitalWriteFast(OLED_DC, LOW); //SET DC LOW FOR COMMAND MODE
    command_or_data = 0;
  }
  //digitalWriteFast(OLED_ERD, HIGH); //SET E high for enable  
  
  PORTK = (byte) Data;
  
  digitalWriteFast(OLED_CS, LOW); //SET E high for enable    
  digitalWriteFast(OLED_CS, HIGH); //SET E high for enable  
  
  //digitalWriteFast(OLED_ERD, LOW); //SET E high for enable  
}


void Write_Data(unsigned char Data){
  if(command_or_data == 0){  
    digitalWriteFast(OLED_DC, HIGH); //SET DC HIGH FOR DATA MODE
    command_or_data = 1;
  }
  //digitalWriteFast(OLED_ERD, HIGH); //SET E high for enable  
  
  PORTK = (byte) Data;
  
  digitalWriteFast(OLED_CS, LOW); //SET E high for enable    
  digitalWriteFast(OLED_CS, HIGH); //SET E high for enable
  
  //digitalWriteFast(OLED_ERD, LOW); //SET E high for enable  
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Instruction Setting
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Exit_Partial_Display( void )
{
   Write_Command(0xA9);
}

void Set_Column_Address(unsigned char a, unsigned char b)
{
	Write_Command(0x15);			// Set Column Address
	Write_Data(a);				//   Default => 0x00
	Write_Data(b);				//   Default => 0x77
}


void Set_Row_Address(unsigned char a, unsigned char b)
{
	Write_Command(0x75);			// Set Row Address
	Write_Data(a);				//   Default => 0x00
	Write_Data(b);				//   Default => 0x7F
}


void Set_Write_RAM()
{
	Write_Command(0x5C);			// Enable MCU to Write into RAM
}


void Set_Read_RAM()
{
	Write_Command(0x5D);			// Enable MCU to Read from RAM
}


void Set_Remap_Format(unsigned char d)
{
	Write_Command(0xA0);			// Set Re-Map / Dual COM Line Mode
	Write_Data(d);				//   Default => 0x40
	  				//     Horizontal Address Increment
						//     Column Address 0 Mapped to SEG0
						//     Disable Nibble Remap
						//     Scan from COM0 to COM[N-1]
						//     Disable COM Split Odd Even
	Write_Data(0x11);			//   Default => 0x01 (Disable Dual COM Mode)
}


void Set_Start_Line(unsigned char d)
{
	Write_Command(0xA1);			// Set Vertical Scroll by RAM
	Write_Data(d);				//   Default => 0x00
}


void Set_Display_Offset(unsigned char d)
{
	Write_Command(0xA2);			// Set Vertical Scroll by Row
	Write_Data(d);				//   Default => 0x00
}


void Set_Display_Mode(unsigned char d)
{
	Write_Command(0xA4|d);			// Set Display Mode
						//   Default => 0xA4
						//     0xA4 (0x00) => Entire Display Off, All Pixels Turn Off
						//     0xA5 (0x01) => Entire Display On, All Pixels Turn On at GS Level 15
						//     0xA6 (0x02) => Normal Display
						//     0xA7 (0x03) => Inverse Display
}


void Set_Partial_Display(unsigned char a, unsigned char b, unsigned char c)
{
	Write_Command(0xA8|a);
						// Default => 0x8F
						//   Select Internal Booster at Display On
	if(a == 0x00)
	{
		Write_Data(b);
		Write_Data(c);
	}
}


void Set_Function_Selection(unsigned char d)
{
	Write_Command(0xAB);			// Function Selection
	Write_Data(d);				//   Default => 0x01
						//     Enable Internal VDD Regulator
}


void Set_Display_On_Off(unsigned char d)
{
	Write_Command(0xAE|d);			// Set Display On/Off
						//   Default => 0xAE
						//     0xAE (0x00) => Display Off (Sleep Mode On)
						//     0xAF (0x01) => Display On (Sleep Mode Off)
}


void Set_Phase_Length(unsigned char d)
{
	Write_Command(0xB1);			// Phase 1 (Reset) & Phase 2 (Pre-Charge) Period Adjustment
	Write_Data(d);				//   Default => 0x74 (7 Display Clocks [Phase 2] / 9 Display Clocks [Phase 1])
						//     D[3:0] => Phase 1 Period in 5~31 Display Clocks
						//     D[7:4] => Phase 2 Period in 3~15 Display Clocks
}


void Set_Display_Clock(unsigned char d)
{
	Write_Command(0xB3);			// Set Display Clock Divider / Oscillator Frequency
	Write_Data(d);				//   Default => 0xD0
						//     A[3:0] => Display Clock Divider
						//     A[7:4] => Oscillator Frequency
}


void Set_Display_Enhancement_A(unsigned char a, unsigned char b)
{
	Write_Command(0xB4);			// Display Enhancement
	Write_Data(0xA0|a);			//   Default => 0xA2
						//     0xA0 (0x00) => Enable External VSL
						//     0xA2 (0x02) => Enable Internal VSL (Kept VSL Pin N.C.)
	Write_Data(0x05|b);			//   Default => 0xB5
						//     0xB5 (0xB0) => Normal
						//     0xFD (0xF8) => Enhance Low Gray Scale Display Quality
}


void Set_GPIO(unsigned char d)
{
	Write_Command(0xB5);			// General Purpose IO
	Write_Data(d);				//   Default => 0x0A (GPIO Pins output Low Level.)
}


void Set_Precharge_Period(unsigned char d)
{
	Write_Command(0xB6);			// Set Second Pre-Charge Period
	Write_Data(d);				//   Default => 0x08 (8 Display Clocks)
}


void Set_Precharge_Voltage(unsigned char d)
{
	Write_Command(0xBB);			// Set Pre-Charge Voltage Level
	Write_Data(d);				//   Default => 0x17 (0.50*VCC)
}


void Set_VCOMH(unsigned char d)
{
	Write_Command(0xBE);			// Set COM Deselect Voltage Level
	Write_Data(d);				//   Default => 0x04 (0.80*VCC)
}


void Set_Contrast_Current(unsigned char d)
{
	Write_Command(0xC1);			// Set Contrast Current
	Write_Data(d);				//   Default => 0x7F
}


void Set_Master_Current(unsigned char d)
{
	Write_Command(0xC7);			// Master Contrast Current Control
	Write_Data(d);				//   Default => 0x0f (Maximum)
}


void Set_Multiplex_Ratio(unsigned char d)
{
	Write_Command(0xCA);			// Set Multiplex Ratio
	Write_Data(d);				//   Default => 0x7F (1/128 Duty)
}


void Set_Display_Enhancement_B(unsigned char d)
{
	Write_Command(0xD1);			// Display Enhancement
	Write_Data(0x82|d);			//   Default => 0xA2
						//     0x82 (0x00) => Reserved
						//     0xA2 (0x20) => Normal
	Write_Data(0x20);
}


void Set_Command_Lock(unsigned char d)
{
	Write_Command(0xFD);			// Set Command Lock
	Write_Data(0x12|d);			//   Default => 0x12
						//     0x12 => Driver IC interface is unlocked from entering command.
						//     0x16 => All Commands are locked except 0xFD.
}



//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_RAM(unsigned char Data)
{
unsigned char i,j;

	//Set_Column_Address(0x00,0x77);
	//Set_Row_Address(0x00,0x7F);
Set_Column_Address(0x00,0x77);
Set_Row_Address(0x00,0x40);

	Set_Write_RAM();

	for(i=0;i<128;i++)
	{
		for(j=0;j<120;j++)
		{
			Write_Data(Data);
			Write_Data(Data);
		}
	}
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Partial or Full Screen)
//
//    a: Column Address of Start
//    b: Column Address of End (Total Columns Devided by 4)
//    c: Row Address of Start
//    d: Row Address of End
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_Block(unsigned char Data, unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
unsigned char i,j;
	
	Set_Column_Address(Shift+a,Shift+b);
	Set_Row_Address(c,d);
	Set_Write_RAM();

	for(i=0;i<(d-c+1);i++)
	{
		for(j=0;j<(b-a+1);j++)
		{
			Write_Data(Data);
			Write_Data(Data);
		}
	}
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Gray Scale Bar (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Grayscale()
{
	// Level 16 => Column 1~16
		Fill_Block(0xFF,0x00,0x03,0x00,Max_Row);

	// Level 15 => Column 17~32
		Fill_Block(0xEE,0x04,0x07,0x00,Max_Row);

	// Level 14 => Column 33~48
		Fill_Block(0xDD,0x08,0x0B,0x00,Max_Row);

	// Level 13 => Column 49~64
		Fill_Block(0xCC,0x0C,0x0F,0x00,Max_Row);

	// Level 12 => Column 65~80
		Fill_Block(0xBB,0x10,0x13,0x00,Max_Row);

	// Level 11 => Column 81~96
		Fill_Block(0xAA,0x14,0x17,0x00,Max_Row);

	// Level 10 => Column 97~112
		Fill_Block(0x99,0x18,0x1B,0x00,Max_Row);

	// Level 9 => Column 113~128
		Fill_Block(0x88,0x1C,0x1F,0x00,Max_Row);

	// Level 8 => Column 129~144
		Fill_Block(0x77,0x20,0x23,0x00,Max_Row);

	// Level 7 => Column 145~160
		Fill_Block(0x66,0x24,0x27,0x00,Max_Row);

	// Level 6 => Column 161~176
		Fill_Block(0x55,0x28,0x2B,0x00,Max_Row);

	// Level 5 => Column 177~192
		Fill_Block(0x44,0x2C,0x2F,0x00,Max_Row);

	// Level 4 => Column 193~208
		Fill_Block(0x33,0x30,0x33,0x00,Max_Row);

	// Level 3 => Column 209~224
		Fill_Block(0x22,0x34,0x37,0x00,Max_Row);

	// Level 2 => Column 225~240
		Fill_Block(0x11,0x38,0x3B,0x00,Max_Row);

	// Level 1 => Column 241~256
		Fill_Block(0x00,0x3C,Max_Column,0x00,Max_Row);
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Character (5x7)
//
//    a: Database
//    b: Ascii
//    c: Start X Address
//    d: Start Y Address
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Show_Font57(unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
unsigned char *Src_Pointer;
unsigned char i,Font,MSB_1,LSB_1,MSB_2,LSB_2;

	switch(a)
	{
		case 1:
        
                        
                        for (uint8_t i =0; i<5; i++ ) {
                          thechar[i] = pgm_read_byte(font+(b*5)+i);
                   
                        }
                        Src_Pointer=&thechar[0];
                        
                        break;
		case 2:
			//Src_Pointer=&Ascii_2[(b-1)][0];
			break;
	}

	Set_Remap_Format(0x15);
	for(i=0;i<=1;i++)
	{
		MSB_1=*Src_Pointer;
		Src_Pointer++;
		if(i == 1)
		{
			LSB_1=0x00;
			MSB_2=0x00;
			LSB_2=0x00;
		}
		else
		{
			LSB_1=*Src_Pointer;
			Src_Pointer++;
			MSB_2=*Src_Pointer;
			Src_Pointer++;
			LSB_2=*Src_Pointer;
			Src_Pointer++;
		}
 		Set_Column_Address(Shift+c,Shift+c);
		Set_Row_Address(d,d+7);
		Set_Write_RAM();

		Font=((MSB_1&0x01)<<4)|(LSB_1&0x01);
		Font=Font|(Font<<1)|(Font<<2)|(Font<<3);
		Write_Data(Font);
		Font=((MSB_2&0x01)<<4)|(LSB_2&0x01);
		Font=Font|(Font<<1)|(Font<<2)|(Font<<3);
		Write_Data(Font);

		Font=((MSB_1&0x02)<<3)|((LSB_1&0x02)>>1);
		Font=Font|(Font<<1)|(Font<<2)|(Font<<3);
		Write_Data(Font);
		Font=((MSB_2&0x02)<<3)|((LSB_2&0x02)>>1);
		Font=Font|(Font<<1)|(Font<<2)|(Font<<3);
		Write_Data(Font);

		Font=((MSB_1&0x04)<<2)|((LSB_1&0x04)>>2);
		Font=Font|(Font<<1)|(Font<<2)|(Font<<3);
		Write_Data(Font);
		Font=((MSB_2&0x04)<<2)|((LSB_2&0x04)>>2);
		Font=Font|(Font<<1)|(Font<<2)|(Font<<3);
		Write_Data(Font);

		Font=((MSB_1&0x08)<<1)|((LSB_1&0x08)>>3);
		Font=Font|(Font<<1)|(Font<<2)|(Font<<3);
		Write_Data(Font);
		Font=((MSB_2&0x08)<<1)|((LSB_2&0x08)>>3);
		Font=Font|(Font<<1)|(Font<<2)|(Font<<3);
		Write_Data(Font);

		Font=((MSB_1&0x10)<<3)|((LSB_1&0x10)>>1);
		Font=Font|(Font>>1)|(Font>>2)|(Font>>3);
		Write_Data(Font);
		Font=((MSB_2&0x10)<<3)|((LSB_2&0x10)>>1);
		Font=Font|(Font>>1)|(Font>>2)|(Font>>3);
		Write_Data(Font);

		Font=((MSB_1&0x20)<<2)|((LSB_1&0x20)>>2);
		Font=Font|(Font>>1)|(Font>>2)|(Font>>3);
		Write_Data(Font);
		Font=((MSB_2&0x20)<<2)|((LSB_2&0x20)>>2);
		Font=Font|(Font>>1)|(Font>>2)|(Font>>3);
		Write_Data(Font);

		Font=((MSB_1&0x40)<<1)|((LSB_1&0x40)>>3);
		Font=Font|(Font>>1)|(Font>>2)|(Font>>3);
		Write_Data(Font);
		Font=((MSB_2&0x40)<<1)|((LSB_2&0x40)>>3);
		Font=Font|(Font>>1)|(Font>>2)|(Font>>3);
		Write_Data(Font);

		Font=(MSB_1&0x80)|((LSB_1&0x80)>>4);
		Font=Font|(Font>>1)|(Font>>2)|(Font>>3);
		Write_Data(Font);
		Font=(MSB_2&0x80)|((LSB_2&0x80)>>4);
		Font=Font|(Font>>1)|(Font>>2)|(Font>>3);
		Write_Data(Font);

		c++;
	}
	Set_Remap_Format(0x14);
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show String
//
//    a: Database
//    b: Start X Address
//    c: Start Y Address
//    * Must write "0" in the end...
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Show_String(unsigned char a, unsigned char *Data_Pointer, unsigned char b, unsigned char c){
  int i;
  unsigned char *Src_Pointer;
  Src_Pointer=Data_Pointer;
  //Show_Font57(1,32,b,c);			// No-Break Space1`					//   Must be written first before the string start...
  while(1){
    Show_Font57(a,*Src_Pointer,b,c);
    Src_Pointer++;
    b+=2;
    if(*Src_Pointer == 0) break;
  }
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Pattern (Partial or Full Screen)
//
//    a: Column Address of Start
//    b: Column Address of End (Total Columns Devided by 4)
//    c: Row Address of Start
//    d: Row Address of End
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Show_Pattern(unsigned char *Data_Pointer, unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
unsigned char *Src_Pointer;
unsigned char i,j;
	
	Src_Pointer=Data_Pointer;
	Set_Column_Address(Shift+a,Shift+b);
	Set_Row_Address(c,d);
	Set_Write_RAM();

	for(i=0;i<(d-c+1);i++)
	{
		for(j=0;j<(b-a+1);j++)
		{
			Write_Data(*Src_Pointer);
			Src_Pointer++;
			Write_Data(*Src_Pointer);
			Src_Pointer++;
		}
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Vertical Scrolling (Full Screen)
//
//    a: Scrolling Direction
//       "0x00" (Upward)
//       "0x01" (Downward)
//    b: Set Numbers of Row Scroll per Step
//    c: Set Time Interval between Each Scroll Step
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Vertical_Scroll(unsigned char a, unsigned char b, unsigned char c){
  unsigned char i,j;	
  Set_Partial_Display(0x00,0x00,Max_Row);
  switch(a){
    case 0:
      for(i=0;i<(Max_Row+1);i+=b){
        Set_Display_Offset(i+1);
        for(j=0;j<c;j++){
          delay(2);
        }
      }
    break;
    case 1:
      for(i=0;i<(Max_Row+1);i+=b){
        Set_Display_Offset(Max_Row-i);
        for(j=0;j<c;j++){
          delay(2);
        }
      }
    break;
  }
  Set_Partial_Display(0x01,0x00,0x00);
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Vertical Fade Scrolling (Full Screen)
//
//    a: Scrolling Direction
//       "0x00" (Upward - In)
//       "0x01" (Downward - In)
//       "0x02" (Upward - Out)
//       "0x03" (Downward - Out)
//    b: Set Numbers of Row Scroll per Step
//    c: Set Time Interval between Each Scroll Step
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fade_Scroll(unsigned char a, unsigned char b, unsigned char c)
{
unsigned char i,j;	

	Set_Partial_Display(0x00,0x00,Max_Row);

	switch(a)
	{
		case 0:
			for(i=(Max_Row+1);i<128;i+=b)
			{
				Set_Start_Line(i);
				for(j=0;j<c;j++)
				{
					delay(2);
				}
			}
			Set_Start_Line(0x00);
			for(j=0;j<c;j++)
			{
				delay(2);
			}
			break;
		case 1:
			for(i=0;i<(Max_Row+1);i+=b)
			{
				Set_Start_Line(Max_Row-i);
				for(j=0;j<c;j++)
				{
					delay(2);
				}
			}
			break;
		case 2:
			for(i=0;i<(Max_Row+1);i+=b)
			{
				Set_Start_Line(i+1);
				for(j=0;j<c;j++)
				{
					delay(2);
				}
			}
			break;
		case 3:
			for(i=127;i>Max_Row;i-=b)
			{
				Set_Start_Line(i);
				for(j=0;j<c;j++)
				{
					delay(2);
				}
			}
			break;
	}
	Set_Partial_Display(0x01,0x00,0x00);
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Fade In (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fade_In()
{
unsigned char i;	

	Set_Display_On_Off(0x01);
	for(i=0;i<(Brightness+1);i++)
	{
		Set_Master_Current(i);
		delay(2);
		delay(2);
		delay(2);
	}
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Fade Out (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fade_Out()
{
unsigned char i;	

	for(i=(Brightness+1);i>0;i--)
	{
		Set_Master_Current(i-1);
		delay(2);
		delay(2);
		delay(2);
	}
	Set_Display_On_Off(0x00);
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Sleep Mode
//
//    "0x01" Enter Sleep Mode
//    "0x00" Exit Sleep Mode
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Sleep(unsigned char a)
{
	switch(a)
	{
		case 0:
			Set_Display_On_Off(0x00);
			Set_Display_Mode(0x01);
			break;
		case 1:
			Set_Display_Mode(0x02);
			Set_Display_On_Off(0x01);
			break;
	}
}

void Set_Linear_Gray_Scale_Table()
{
	Write_Command(0xB9);			// Set Default Linear Gray Scale Table
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Gray Scale Table Setting (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Set_Gray_Scale_Table()
{
	oled_Command_25664(0xB8);			// Set Gray Scale Table
	oled_Data_25664(0x0C);			//   Gray Scale Level 1
	oled_Data_25664(0x18);			//   Gray Scale Level 2
	oled_Data_25664(0x24);			//   Gray Scale Level 3
	oled_Data_25664(0x30);			//   Gray Scale Level 4
	oled_Data_25664(0x3C);			//   Gray Scale Level 5
	oled_Data_25664(0x48);			//   Gray Scale Level 6
	oled_Data_25664(0x54);			//   Gray Scale Level 7
	oled_Data_25664(0x60);			//   Gray Scale Level 8
	oled_Data_25664(0x6C);			//   Gray Scale Level 9
	oled_Data_25664(0x78);			//   Gray Scale Level 10
	oled_Data_25664(0x84);			//   Gray Scale Level 11
	oled_Data_25664(0x90);			//   Gray Scale Level 12
	oled_Data_25664(0x9C);			//   Gray Scale Level 13
	oled_Data_25664(0xA8);			//   Gray Scale Level 14
	oled_Data_25664(0xB4);			//   Gray Scale Level 15

	oled_Command_25664(0x00);			// Enable Gray Scale Table
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Initialization
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void OLED_Init(){
  unsigned char i;
  delay(100);
  digitalWrite(OLED_RES, LOW);
  delay(400);
  digitalWrite(OLED_RES, HIGH);

  Set_Command_Lock(0x12);			// Unlock Basic Commands (0x12/0x16)
  Set_Display_On_Off(0x00);		// Display Off (0x00/0x01)
  Set_Column_Address( 0x1C, 0x5B );
  Set_Row_Address( 0x00, 0x3F );
  Set_Display_Clock(0x91);		// Set Clock as 80 Frames/Sec
  Set_Multiplex_Ratio(0x3F);		// 1/64 Duty (0x0F~0x3F)
  Set_Display_Offset(0x00);		// Shift Mapping RAM Counter (0x00~0x3F)
  Set_Start_Line(0x00);			// Set Mapping RAM Display Start Line (0x00~0x7F)
  Set_Remap_Format(0x14);			// Set Horizontal Address Increment
						//     Column Address 0 Mapped to SEG0
						//     Disable Nibble Remap
						//     Scan from COM[N-1] to COM0
						//     Disable COM Split Odd Even
						//     Enable Dual COM Line Mode
  Set_GPIO(0x00);				// Disable GPIO Pins Input
  Set_Function_Selection(0x01);		// Enable Internal VDD Regulator
  Set_Display_Enhancement_A(0xA0,0xFD);	
  					
  Set_Contrast_Current(0x9F);		// Set Segment Output Current
  Set_Master_Current(0x0F);		// Set Scale Factor of Segment Output Current Control
//  Set_Gray_Scale_Table();			// Set Pulse Width for Gray Scale Table
  Set_Linear_Gray_Scale_Table();
  Set_Phase_Length(0xE2);			// Set Phase 1 as 5 Clocks & Phase 2 as 14 Clocks
  Set_Display_Enhancement_B(0x20);	// Enhance Driving Scheme Capability (0x00/0x20)
  Set_Precharge_Voltage(0x1F);		// Set Pre-Charge Voltage Level as 0.60*VCC
  Set_Precharge_Period(0x08);		// Set Second Pre-Charge Period as 8 Clocks
  Set_VCOMH(0x07);			// Set Common Pins Deselect Voltage Level as 0.86*VCC
  Set_Display_Mode(0x02);			// Normal Display Mode (0x00/0x01/0x02/0x03)
  Set_Partial_Display(0x01,0x00,0x00);	// Disable Partial Display
  //Exit_Partial_Display();
  
  

  Set_Display_On_Off(0x01);		// Display On (0x00/0x01)
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Checkboard (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Checkerboard()
{
unsigned char i,j;
	
	Set_Column_Address(0x00,0x77);
	Set_Row_Address(0x00,0x7F);
	Set_Write_RAM();

	for(i=0;i<64;i++)
	{
		for(j=0;j<120;j++)
		{
			Write_Data(0xF0);
			Write_Data(0xF0);
		}
		for(j=0;j<120;j++)
		{
			Write_Data(0x0F);
			Write_Data(0x0F);
		}
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Graphic Acceleration (Partial or Full Screen)
//
//    a: Line Width
//    b: Column Address of Start
//    c: Column Address of End
//    d: Row Address of Start
//    e: Row Address of End
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Draw_Rectangle(unsigned char Data, unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e){
  unsigned char i,j,k,l;
  k=a%4;
  if(k == 0){
    l=(a/4)-1;
  }
  else{
    l=a/4;
  }

  Set_Column_Address(Shift+b,Shift+c);
  Set_Row_Address(d,(d+a-1));
  Set_Write_RAM();
  for(i=0;i<(c-b+1);i++){
    for(j=0;j<a;j++){
      Write_Data(Data);
      Write_Data(Data);
    }
  }

  Set_Column_Address(Shift+(c-l),Shift+c);
  Set_Row_Address(d+a,e-a);
  Set_Write_RAM();
  for(i=0;i<(e-d+1);i++){
    for(j=0;j<(l+1);j++){
      if(j == 0){
        switch(k){
	  case 0:
	    Write_Data(Data);
	    Write_Data(Data);
	  break;
	  case 1:
	    Write_Data(0x00);
	    Write_Data(Data&0x0F);
          break;
	  case 2:
	    Write_Data(0x00);
	    Write_Data(Data);
	  break;
	  case 3:
	    Write_Data(Data&0x0F);
	    Write_Data(Data);
	  break;
        }
      }
      else{
        Write_Data(Data);
        Write_Data(Data);
      }
    }
  }

  Set_Column_Address(Shift+b,Shift+c);
  Set_Row_Address((e-a+1),e);
  Set_Write_RAM();
  for(i=0;i<(c-b+1);i++){
    for(j=0;j<a;j++){
      Write_Data(Data);
      Write_Data(Data);
    }
  }

  Set_Column_Address(Shift+b,Shift+(b+l));
  Set_Row_Address(d+a,e-a);
  Set_Write_RAM();
  for(i=0;i<(e-d+1);i++){
    for(j=0;j<(l+1);j++){
      if(j == l){
	switch(k){
	  case 0:
	    Write_Data(Data);
	    Write_Data(Data);
	  break;
	  case 1:
	  Write_Data(Data&0xF0);
	  Write_Data(0x00);
	break;
	case 2:
	  Write_Data(Data);
	  Write_Data(0x00);
	break;
	case 3:
	  Write_Data(Data);
	  Write_Data(Data&0xF0);
	break;
      }
    }
    else{
      Write_Data(Data);
      Write_Data(Data);
    }
  }
  }
}

/***********************************************

END OLED STUFF

*************************************************/


