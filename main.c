/*!**********************************************************************************************************************
@file main.c                                                                
@brief Main system file for the ENCM 369 firmware.  
***********************************************************************************************************************/

#include "configuration.h"


/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32SystemTime1ms = 0;     /*!< @brief Global system time incremented every ms, max 2^32 (~49 days) */
volatile u32 G_u32SystemTime1s  = 0;     /*!< @brief Global system time incremented every second, max 2^32 (~136 years) */
volatile u32 G_u32SystemFlags   = 0;     /*!< @brief Global system flags */

/*--------------------------------------------------------------------------------------------------------------------*/
/* External global variables defined in other files (must indicate which file they are defined in) */

extern u8 G_au8Time[];
extern u8 G_au8AlarmTime[];

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Main_" and be declared as static.
***********************************************************************************************************************/


//BCD decoding data, note bit 7 is g, 5-0 bits are f,e,d,c,b, and a.
//This is because my tired brain wired it backwards and for some reason LATA's 6th bit, 
//2nd MSB,doesn't work properly
__EEPROM_DATA(0b00111111, 0b00000110, 0b10011011, 0b10001111, 0b10100110, 0b10101101, 0b10111101, 0b00000111);
__EEPROM_DATA(0b10111111, 0b10101111, 0b10110111, 0b10111100, 0b10111100, 0b00111001, 0b10111001, 0b10110001);


/*!**********************************************************************************************************************
@fn void main(void)
@brief Main program where all tasks are initialized and executed.


***********************************************************************************************************************/

void main(void)
{ 
    G_u32SystemFlags |= _SYSTEM_INITIALIZING;

  /* Low level initialization */
  ClockSetup();
  SysTickSetup();
  GpioSetup();
  
  /* Driver initialization */
  //SegmentDecoderIntialize();
  
  /* Application initialization */
  UserAppInitialize();
  
  /* Exit initialization */
    
  /* Super loop */  
  while(1)
  {
    /* Drivers */
       
    /* Applications */
    UserAppRun();
    
    
    
    /* System sleep */
    //HEARTBEAT_OFF();
    SystemSleep();
    TimeXus(1); //Update every ~3ms 
    while((PIR3 & 0x80) == 0x00); 
    
    static u8 u8DigitCounter = 0;  //Setting digit display, digits 1-4
    static u8 u8TimeCounter = 0;  //Setting time array focus
    
    if(u8DigitCounter == 0) // Time storing conventions are hard and obtuse to reach with a single loop, could have reworked but brain too small
    {
        NVMADR = 0x380000 + ((G_au8Time[2] >> 4) & 0x0F);   //Print seconds 
        LATB = 0x01;
        u8DigitCounter++;
    } 
    else if(u8DigitCounter == 1)
    {
        NVMADR = 0x380000 + ((G_au8Time[1] >> 0) & 0x0F);   //Print 10s of seconds
        LATB = 0x02;
        u8DigitCounter++;
    }
    else if(u8DigitCounter == 2)
    {
        NVMADR = 0x380000 + ((G_au8Time[1] >> 4) & 0x0F);    //Print mins
        LATB = 0x04;
        u8DigitCounter++;
    }
    else if(u8DigitCounter == 3)
    {
        NVMADR = 0x380000 + ((G_au8Time[0] >> 0) & 0x07);    //Print 10s of mins
        LATB = 0x08;
        u8DigitCounter = 0;
    }
    
    NVMCON1bits.CMD = 0x00; //Setting NVM configuration bits to read word
    NVMCON0bits.GO = 1;     //Read word
    while (NVMCON0bits.GO); //Waiting until byte is read
    u8 u8BCDecode = NVMDATL;    //reading every cycle might be better to store data and adjust if different
    LATA = (LATA & 0x40) + u8BCDecode; 
    
    //HEARTBEAT_ON();
  }
} /* end main() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/

