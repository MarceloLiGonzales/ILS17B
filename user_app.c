/*!*********************************************************************************************************************
@file user_app.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void UserApp1Initialize(void)
- void UserApp1Run(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u8 G_u8UserAppFlags;                             /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp_<type>" and be declared as static.
***********************************************************************************************************************/


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void UserAppInitialize(void)

@brief
Initializes the application's variables.

Should only be called once in main init section.

Requires:
- RA0-7 setup as digital output

Promises:
- For RA7 to be high
- For timer0 to be enabled in async. 16-bit mode with a prescaler of 1:16 for the (Fosc/4) source.

*/
void UserAppInitialize(void)
{

    OSCCON3bits.SOSCPWR = 0;     //Setting power mode to Low Power for 32.768Hz Quartz Crystals
    OSCENbits.SOSCEN = 1;       //Enabling secondary oscillator
    
    //Waiting until secondary oscillator is setup since sometimes failure can happen if crystal is bad.
    while(OSCSTATbits.SOR != 1)
    {
    }
    
    //LED initialization
    LATA = 0x80; //Setting RA7 latch to digital high, and RA0-6 latches low
    
    T0CON1 = 0b11011101;    //Setting timer0 to asynchronous. mode with (SOSC) as the source with a pre-scaler of 1:8192, 0.25s
    T0CON0 = 0b11000000;    //Enabling timer0 in 16-bit mode with a post-scaler value of 1:1


} /* end UserAppInitialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserAppRun(void)

@brief Application code that runs once per system loop

Requires:+
- Time to pass normally

Promises:
- RA06
- Time keeping, should actually use global variables that are indexable outside of here
- ...so that time display can occur
*/
void UserAppRun(void)
{
    /* time is setup up like this (Might not seem ideal but i think it works fine, small sizes are nice since only 1 extra bit present in the 10's of seconds): 
       Time1: (1-bit) 10's of hours | (4-bits) hours | (3-bits) 10's of minutes
       Time2: (4-bits) minutes | (4-bits) 10's of seconds
       Time3: (4-bits) seconds | ((4-bits) NULL
        
     Each bit section is purely the value of the place so 53 minutes means 5 in the 10's of minutes and 3 in the minutes
    */
    //setting time to be 09:59.59
    static u8 u8Time1 = 0b01001101;
    static u8 u8Time2 = 0b10010101;
    static u8 u8Time3 = 0b10010000;
    
    
    if(u8Time3 == 0x90) //Is seconds = 9?
    {
        if((u8Time2 & 0x0F) == 0x05) //Is seconds = 59?
        {
            if(u8Time2 == 0x95) //Is minutes 9.59?
            {
                if((u8Time1 & 0x07) == 0b00000101 ) //Is minutes 59.59?
                {
                    if(u8Time1 > 0x80)  //is it (10-12):59.59?, (different cases of time switching)
                    {
                        if(u8Time1 == 0b11001101)    //is it 12:59.59 ?
                        {
                            u8Time1 = 0b00001000; //Setting time to 01:00.00
                        }
                        else
                        {
                            u8Time1 += 0x08; //incrementing hours
                        }
                    }
                    else
                    {
                        if(u8Time1 == 0x4D) //is it 9:59.59?
                        {
                            u8Time1 = 0x80; //Setting time to 10:00.00
                        }
                        else
                        {
                            u8Time1 += 0x08; //incrementing hours
                        }
                    }
                }
                else
                {
                    u8Time1 += 0x01;    //increment 10's of minutes
                }
                u8Time2 = 0x00;    //Set minutes and 10's seconds to zero
            }
            else
            {
                u8Time2 = (u8Time2 & 0xF0) + 0x10;  //increment minutes and set 10's seconds to 0
            }
        }
        else
        {
            u8Time2 += 0x01;  //increment tens of seconds
        }
        u8Time3 = 0x00;   //set seconds to zero
    }
    else
    {
        u8Time3 += 0x10; //increment seconds
    }
    
    LATA ^=0x40;    //blinky that doesn't work since LATA06 is dumb
} /* end UserAppRun */

/*!--------------------------------------------------------------------------------------------------------------------
@fn void TimeXus(u16 u16TimeXus)

@brief Sets Timer0 to count u16Microseconds_

Requires:
- Timer0 configured
- u16 u16TimeXus is the time of (osc. period/pre-scaler*post-scaler)seconds  

Promises:
- Pre-loads TMR0H:L to clock out desired period
- TMR0IF cleared
- Timer0 enabled

*/

void TimeXus(u16 u16TimerTime)
{
    T0CON0 &= 0x7F; //Disabling timer0
    
    TMR0H = (0xFFFF - u16TimerTime) >> 8; /*Right shifting upper 8 bits of u16TimerTime 
                               to fit in 8-bit TMR0H*/ 
    
    //Pre-loading TMR0L/H based on passed u16TimerTime
    TMR0L = (0xFFFF - u16TimerTime) & 0x00FF; /*Taking only the lower 8 bits of u16TimerTime
                                               for TMR0L*/  
    
    PIR3 &= 0x7F; //Clearing the TMR0IF flag
    T0CON0 |= 0x80; //Enabling timer0
    
    
}  /* end TimeXus () */

/*!--------------------------------------------------------------------------------------------------------------------
@fn void SegmentDecoderIntialize(void)

@brief
Initializes the EEPROM data for use as a lookup table for 7 segment displays

Should only be called once in main init section.

Requires:
- Zach to be a good embedded programmer; I can't btw.

Promises:
- For addresses 380000 - 380010 to have EEPROM values
- Data programmed is hex words for a segment a-f in that order
- Data is programmed at start if not present, configure PICKit settings not to wipe EEPROM data and use user defined memory ranges

*/
void SegmentDecoderIntialize(void)
{
    NVMADR = 380010;        //Setting NVM address to data state word address
    NVMCON1bits.CMD = 0x00; //Setting NVM configuration bits to read word
    NVMCON0bits.GO = 1;     //Read word
    while (NVMCON0bits.GO); //Waiting until byte is read
    
    if(1)   //data state word = 0x0000 if programmed already
    {
        /*Since data is not programmed programming data to EEPROM
        MUST erase words before modifying however, 
        can't erase 1 word must erase whole page*/
        uint16_t bufferRAM = 0x2500;  
        uint16_t *bufferRamPtr = (uint16_t*) & bufferRAM; // Defining a pointer to the first location of the Buffer RAM
        
        //Reading existing EEPROM page
        NVMADR = 0x380000;            //Setting NVM address to start of EEPROM
        NVMCON1bits.CMD = 0x02;     //Configuration bits: read entire page to buffer RAM
        INTCON0bits.GIE = 0;        //Disabling interrupts while reading since busy
        NVMCON0bits.GO = 1;         //Perform read operation
        while (NVMCON0bits.GO);     //Waiting until read is done
        
        //Erasing EEPROM page
        NVMCON1bits.CMD = 0x06;     //Configuration bits: erase entire page
        NVMLOCK = 0x55;             //Unlock sequence needed to write data
        NVMLOCK = 0xAA;
        NVMCON0bits.GO = 1;         //Erasing page
        while (NVMCON0bits.GO);     //Waiting until erase is done
        
        if(NVMCON1bits.WRERR)        //Checking if erase was successful
        {
            //Erase was bad staying in loop
            //might have failsafe function at one point?
        }
        
        //Editing PAGE in buffer ram
        u8 u8ArrayIndex = 0;
        //Array 2-bytes words contain the decoded segment bytes for each address offset and 
        u16 au16DecodingValues[] = {0x3F06, 0x5B4F, 0x666D, 0x7D07,  
                                  0x7F6F, 0x777C, 0x395E, 0x7971, 0x0000};
        
        //u8 u8DataOffset = u8 (0x380000 & ((0x1024 * 2) - 1) /2); //offset
        u8 u8Offset = (u8) ((0x380000 & ((128 * 2) - 1)) / 2); 
        while (u8ArrayIndex < 0x09)
        {
            *bufferRamPtr = au16DecodingValues[u8ArrayIndex];
            bufferRamPtr++; //might work??? I think it will??
            //this does not work I cant figure out how to edit buffer ram data
            //it jumps all over the place and I cant find debugging location of 
            //buffer ram????
            //The write after this fails probbaly because the data is fucked up
            //Might come back and use TABLE functions like it suggest, not much info online nor in the datasheet on exact usage
            u8ArrayIndex++;
        }
        *bufferRamPtr = 0x0000;
        
        //Write EEPROM page
        NVMADR = 0x380000;
        NVMCON1bits.CMD = 0b101;     //Configuration bits write page
        NVMLOCK = 0x55;             //Unlock sequence to write data
        NVMLOCK = 0xAA;
        NVMCON0bits.GO = 1;         //Writing page
        while (NVMCON0bits.GO);     //Waiting until write is done
        
        if(NVMCON1bits.WRERR)        //Checking if write was successful
        {
            while(1){};
            //Write was bad staying in loop
            //might have failsafe function at one point?
        }
        
        INTCON0bits.GIE = 1;        //Disabling interrupts while reading since busy
        NVMCON1bits.CMD = 0x00;     //Disabling writes to memory
    }
    
    
    
    

} /* end SegmentDecoderIntialize() */



/*--------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
