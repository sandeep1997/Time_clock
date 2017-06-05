#ifndef NokiaDisplay_h
#define NokiaDisplay_h

#define LEFT 0
#define RIGHT 9999
#define CENTER 9998

#define LCD_COMMAND 0
#define LCD_DATA 1

// PCD8544 Commandset
// ------------------
// General commands
#define PCD8544_POWERDOWN      0x04
#define PCD8544_ENTRYMODE     0x02
#define PCD8544_EXTENDEDINSTRUCTION 0x01
#define PCD8544_DISPLAYBLANK    0x00
#define PCD8544_DISPLAYNORMAL   0x04
#define PCD8544_DISPLAYALLON    0x01
#define PCD8544_DISPLAYINVERTED   0x05
// Normal instruction set
#define PCD8544_FUNCTIONSET     0x20
#define PCD8544_DISPLAYCONTROL    0x08
#define PCD8544_SETYADDR      0x40
#define PCD8544_SETXADDR      0x80
// Extended instruction set
#define PCD8544_SETTEMP       0x04
#define PCD8544_SETBIAS       0x10
#define PCD8544_SETVOP        0x80
// Display presets
#define LCD_BIAS          0x03  // Range: 0-7 (0x00-0x07)
#define LCD_TEMP          0x02  // Range: 0-3 (0x00-0x03)
#define LCD_CONTRAST        0x46  // Range: 0-127 (0x00-0x7F)
#if defined(__AVR__)
  #include "Arduino.h"
  #include "hardware/avr/HW_AVR_defines.h"
#elif defined(__PIC32MX__)
  #include "WProgram.h"
  #include "hardware/pic32/HW_PIC32_defines.h"
#elif defined(__arm__)
  #include "Arduino.h"
  #include "hardware/arm/HW_ARM_defines.h"
#endif

struct _current_font
{
  uint8_t* font;
  uint8_t x_size;
  uint8_t y_size;
  uint8_t offset;
  uint8_t numchars;
  uint8_t inverted;
};

// blueprint 
class LCD5110 {
   public:
      
    
   // Methods
   
    LCD5110(int SCK, int MOSI, int DC, int RST, int CS);
    void InitLCD(int contrast=LCD_CONTRAST);
    void update();
    void clrScr();
    void print(String st, int x, int y);
    void printNumI(long num, int x, int y, int length=0, char filler=' ');
    void setFont(uint8_t* font);
    void setPixel(uint16_t x, uint16_t y);
    void clrPixel(uint16_t x, uint16_t y);
    void state(int,int,int);

 protected:
    regtype     *P_SCK, *P_MOSI, *P_DC, *P_RST, *P_CS;
    regsize     B_SCK, B_MOSI, B_DC, B_RST, B_CS;
    _current_font  cfont;
    uint8_t     SCK_Pin, RST_Pin;        
    uint8_t     scrbuf[504];
    boolean     _sleep;
    int       _contrast;
    void _LCD_Write(unsigned char data, unsigned char mode);
    void _print_char(unsigned char c, int x, int row);
    void _convert_float(char *buf, double num, int width, byte prec);



  private:  
         
};

#endif
