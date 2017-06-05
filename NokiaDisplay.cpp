#include "NokiaDisplay.h"

#if defined(__AVR__)
  #include <avr/pgmspace.h>
  #include "hardware/avr/HW_AVR.h"
#elif defined(__PIC32MX__)
  #pragma message("Compiling for PIC32 Architecture...")
  #include "hardware/pic32/HW_PIC32.h"
#elif defined(__arm__)
  #pragma message("Compiling for ARM Architecture...")
  #include "hardware/arm/HW_ARM.h"
#endif



LCD5110::LCD5110(int SCK, int MOSI, int DC, int RST, int CS)
{ 
  P_SCK = portOutputRegister(digitalPinToPort(SCK));
  B_SCK = digitalPinToBitMask(SCK);
  P_MOSI  = portOutputRegister(digitalPinToPort(MOSI));
  B_MOSI  = digitalPinToBitMask(MOSI);
  P_DC  = portOutputRegister(digitalPinToPort(DC));
  B_DC  = digitalPinToBitMask(DC);
  P_RST = portOutputRegister(digitalPinToPort(RST));
  B_RST = digitalPinToBitMask(RST);
  P_CS  = portOutputRegister(digitalPinToPort(CS));
  B_CS  = digitalPinToBitMask(CS);
  pinMode(SCK,OUTPUT);
  pinMode(MOSI,OUTPUT);
  pinMode(DC,OUTPUT);
  pinMode(RST,OUTPUT);
  pinMode(CS,OUTPUT);
  SCK_Pin=SCK;
  RST_Pin=RST;
}
void LCD5110::_LCD_Write(unsigned char data, unsigned char mode)
{   
    cbi(P_CS, B_CS);

  if (mode==LCD_COMMAND)
    cbi(P_DC, B_DC);
  else
    sbi(P_DC, B_DC);

  for (unsigned char c=0; c<8; c++)
  {
    if (data & 0x80)
      sbi(P_MOSI, B_MOSI);
    else
      cbi(P_MOSI, B_MOSI);
    data = data<<1;
    pulseClock;
  }

  sbi(P_CS, B_CS);
}


void LCD5110::InitLCD(int contrast)
{
  if (contrast>0x7F)
    contrast=0x7F;
  if (contrast<0)
    contrast=0;

  resetLCD;

  _LCD_Write(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION, LCD_COMMAND);
  _LCD_Write(PCD8544_SETVOP | contrast, LCD_COMMAND);
  _LCD_Write(PCD8544_SETTEMP | LCD_TEMP, LCD_COMMAND);
  _LCD_Write(PCD8544_SETBIAS | LCD_BIAS, LCD_COMMAND);
  _LCD_Write(PCD8544_FUNCTIONSET, LCD_COMMAND);
  _LCD_Write(PCD8544_SETYADDR, LCD_COMMAND);
  _LCD_Write(PCD8544_SETXADDR, LCD_COMMAND);
  _LCD_Write(PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL, LCD_COMMAND);

  clrScr();
  update();
  cfont.font=0;
  _sleep=false;
  _contrast=contrast;
}

void LCD5110::setFont(uint8_t* font)
{
  cfont.font=font;
  cfont.x_size=fontbyte(0);
  cfont.y_size=fontbyte(1);
  cfont.offset=fontbyte(2);
  cfont.numchars=fontbyte(3);
  cfont.inverted=0;
}

void LCD5110::update()
{
  if (_sleep==false)
  {
    _LCD_Write(PCD8544_SETYADDR, LCD_COMMAND);
    _LCD_Write(PCD8544_SETXADDR, LCD_COMMAND);
    for (int b=0; b<504; b++)
      _LCD_Write(scrbuf[b], LCD_DATA);
  }
}

void LCD5110::clrScr()
{
  for (int c=0; c<504; c++)
    scrbuf[c]=0;
}

void LCD5110::setPixel(uint16_t x, uint16_t y)
{
  int by, bi;

  if ((x>=0) and (x<84) and (y>=0) and (y<48))
  {
    by=((y/8)*84)+x;
    bi=y % 8;

    scrbuf[by]=scrbuf[by] | (1<<bi);
  }
}

void LCD5110::clrPixel(uint16_t x, uint16_t y)
{
  int by, bi;

  if ((x>=0) and (x<84) and (y>=0) and (y<48))
  {
    by=((y/8)*84)+x;
    bi=y % 8;

    scrbuf[by]=scrbuf[by] & ~(1<<bi);
  }
}


void LCD5110::_print_char(unsigned char c, int x, int y)
{
  if ((cfont.y_size % 8) == 0)
  {
    int font_idx = ((c - cfont.offset)*(cfont.x_size*(cfont.y_size/8)))+4;
    for (int rowcnt=0; rowcnt<(cfont.y_size/8); rowcnt++)
    {
      for(int cnt=0; cnt<cfont.x_size; cnt++)
      {
        for (int b=0; b<8; b++)
          if ((fontbyte(font_idx+cnt+(rowcnt*cfont.x_size)) & (1<<b))!=0)
            if (cfont.inverted==0)
              setPixel(x+cnt, y+(rowcnt*8)+b);
            else
              clrPixel(x+cnt, y+(rowcnt*8)+b);
          else
            if (cfont.inverted==0)
              clrPixel(x+cnt, y+(rowcnt*8)+b);
            else
              setPixel(x+cnt, y+(rowcnt*8)+b);
      }
    }
  }
  else
  {
    int font_idx = ((c - cfont.offset)*((cfont.x_size*cfont.y_size/8)))+4;
    int cbyte=fontbyte(font_idx);
    int cbit=7;
    for (int cx=0; cx<cfont.x_size; cx++)
    {
      for (int cy=0; cy<cfont.y_size; cy++)
      {
        if ((cbyte & (1<<cbit)) != 0)
          if (cfont.inverted==0)
            setPixel(x+cx, y+cy);
          else
            clrPixel(x+cx, y+cy);
        else
          if (cfont.inverted==0)
            clrPixel(x+cx, y+cy);
          else
            setPixel(x+cx, y+cy);
        cbit--;
        if (cbit<0)
        {
          cbit=7;
          font_idx++;
          cbyte=fontbyte(font_idx);
        }
      }
    }
  }
}



void LCD5110::print(String st, int x, int y)
{
  char buf[st.length()+1];

  st.toCharArray(buf, st.length()+1);
  print(buf, x, y);
}

void LCD5110::printNumI(long num, int x, int y, int length, char filler)
{
  char buf[25];
  char st[27];
  boolean neg=false;
  int c=0, f=0;
  
  if (num==0)
  {
    if (length!=0)
    {
      for (c=0; c<(length-1); c++)
        st[c]=filler;
      st[c]=48;
      st[c+1]=0;
    }
    else
    {
      st[0]=48;
      st[1]=0;
    }
  }
  else
  {
    if (num<0)
    {
      neg=true;
      num=-num;
    }
    
    while (num>0)
    {
      buf[c]=48+(num % 10);
      c++;
      num=(num-(num % 10))/10;
    }
    buf[c]=0;
    
    if (neg)
    {
      st[0]=45;
    }
    
    if (length>(c+neg))
    {
      for (int i=0; i<(length-c-neg); i++)
      {
        st[i+neg]=filler;
        f++;
      }
    }

    for (int i=0; i<c; i++)
    {
      st[i+neg+f]=buf[c-i-1];
    }
    st[c+neg+f]=0;

  }

  print(st,x,y);
}

void LCD5110::state(int hr,int mint,int sec)
{     
     printNumI(hr,23,25); 
     update();
     print(":",52,25);
     update();
     printNumI(mint,40,25);
     update();
     print(":",35,25);
     update();
     printNumI(sec,54,25);
     update();
      
}

