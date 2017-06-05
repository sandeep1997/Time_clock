#include<Button.h>
#include "NokiaDisplay.h"

LCD5110 lcd(8,9,10,12,11);                                      // calling nokia display constructor

int hr=0;
int mint=0;
int sec=0;

extern unsigned char SmallFont[];  
Button mode = Button(6,PULLUP);                                 // mode changer 
Button mintue = Button(5,PULLUP);                               // mintue changer 
Button hour = Button(4,PULLUP);                                 // hour changer 


void change(){
  
     do
     {       
        if(hour.isPressed()==true)
        {
           delay(500);
           if(hr<=12)
             lcd.state(hr++,mint,sec);
           else
           {
             hr=0;
             lcd.clrScr();
             lcd.state(hr,mint,sec);   
           }
        }
  
        if(mintue.isPressed()==true)
        {
          delay(500);
          if(mint<=59)
            lcd.state(hr,mint++,sec);
          else
          {
            lcd.clrScr();
            lcd.state(hr++,0,0);  
            mint=0;
            sec=0;
          }
        }
                 
     }while(mode.isPressed()!=true);

}
 
void start()
{
  // for display zero condition
   lcd.state(hr,mint,sec);
   
  do
  {
    for(sec=0;sec<61;sec++)
    {
      
       if(mode.isPressed()==true)          // if mode is pressed the current time is passed and change function is executed 
         {
          delay(1000);                     // one second delay for debouncing
          change();
         }
                   
        if(sec==59)                        // after 60 second sec=0 and mint++
        {   
          mint++;    
          if(mint==60)
          {
            hr++;
            mint=0;
          }
        }
      delay(1000);                         // one second delay = 1sec 
      lcd.clrScr();
      lcd.state(hr,mint,sec);              // to print the current state
        
    }
  }while(mode.isPressed()==true);
  
}


 
void setup(){
  
   lcd.InitLCD();
   lcd.setFont(SmallFont);
   lcd.print("CLOCK", CENTER, 25);
   lcd.update();
   delay(2000);
   lcd.clrScr();
   start();
}


void loop()
{
   
}


