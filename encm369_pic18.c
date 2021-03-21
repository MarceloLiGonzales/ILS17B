/*!**********************************************************************************************************************
@file encm369_pic18.c                                                                
@brief This file provides core and GPIO functions for the ENCM 369 PIC activities.


------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- 

PROTECTED FUNCTIONS
- 

***********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_xxBsp"
***********************************************************************************************************************/
/* New variables */




/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;        /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;         /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;          /*!< @brief From main.c */

extern u8 G_u8TimeFlag;
/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Bsp_" and be declared as static.
***********************************************************************************************************************/

/***********************************************************************************************************************
Function Definitions
***********************************************************************************************************************/


/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!---------------------------------------------------------------------------------------------------------------------
@fn void INTERRUPTInitialize(void)

@brief Setups interrupts and interrupt priority

Requires:
- NONE

Promises:
- TMR0 interrupt is enabled as a low priority interrupt

*/
void INTERRUPTInitialize(void)
{
    INTCON0bits.IPEN = 1;   //Enabling interrupts
    IPR3bits.TMR0IP = 0;    //Setting TMR0 interrupt priority to low default high
    
    PIR3bits.TMR0IF = 0;    //Clearing the TMR0IF flag
    
    PIE3bits.TMR0IE = 1;    //Enabling TMR0 interrupt ability
    
    INTCON0bits.GIEH = 1;   //Enabling high-priority unmasked interrupts
    INTCON0bits.GIEL = 1;   //Enabling low-priority unmasked interrupts
    
}/* end INTERRUPTInitialize */


/*!---------------------------------------------------------------------------------------------------------------------
@fn void __interrupt(irq(IRQ_TMR0), low_priority) TMR0_ISR(void)

@brief Setups interrupts and interrupt priority

Requires:
- To be called via Vector Table from an interrupt event

Promises:
- To respond to the interrupt

*/
void __interrupt(irq(IRQ_TMR0), low_priority) TMR0_ISR(void)
{
    PIR3bits.TMR0IF = 0;    //Clearing the TMR0IF flag
    T0CON0 &= 0x7F; //Disabling timer0
    
    TMR0H = (0xFFFF - 0x0001) >> 8; 
    
    //Pre-loading TMR0L/H 
    TMR0L = (0xFFFF - 0x0001) & 0x00FF;  
    
    T0CON0 |= 0x80; //Enabling timer
    G_u8TimeFlag = 0xFF;
}/* end __interrupt */


/*!---------------------------------------------------------------------------------------------------------------------
@fn void ClockSetup(void)

@brief Loads all registers required to set up the processor clocks.

Requires:
- NONE

Promises:
- EFC is set up with proper flash access wait states based on 48MHz system clock
- PMC is set up with proper oscillators and clock sources

*/
void ClockSetup(void)
{
 
} /* end ClockSetup */


/*!---------------------------------------------------------------------------------------------------------------------
@fn void GpioSetup(void)

@brief Loads registers required to set up GPIO on the processor.

Requires:
- All configurations must match connected hardware.

Promises:
- Output pin for PA31_HEARTBEAT is configured
- RA0-7 configured for digital output, initial value 0x00
- RB0-7 configured for digital output initial value 0x00
*/
void GpioSetup(void)
{
    ANSELA = 0x00; //Setting up RA0-7 as digital IO
    TRISA = 0x00; //Setting RA0-7 to be outputs
    LATA  = 0x00; //Setting RA0-7 to be default off/low
    
    ANSELB = 0x00; //Setting up RB0-7 as digital IO
    TRISB = 0x00; //Setting RB0-7 to be outputs
    LATB  = 0x00; //Setting RB0-7 to be default off/low
} /* end GpioSetup() */


/*!---------------------------------------------------------------------------------------------------------------------
@fn  void SysTickSetup(void)

@brief Initializes the 1ms and 1s System Ticks off the core timer.

Requires:
- NVIC is setup and SysTick handler is installed

Promises:
- Both global system timers are reset and the SysTick core timer is configured for 1ms intervals

*/
void SysTickSetup(void)
{
  G_u32SystemTime1ms = 0;      
  G_u32SystemTime1s  = 0;   
  
} /* end SysTickSetup() */



/*!---------------------------------------------------------------------------------------------------------------------
@fn void SystemSleep(void)

@brief Puts the system into sleep mode.  


Requires:
- 
 * 
Promises:
- 

*/
void SystemSleep(void)
{    
  
} /* end SystemSleep(void) */



/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/



