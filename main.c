/* 
 * File:   main.c
 * Project : Cooperative scheduler for Digilent MAX32
 * Author: Daniel McBrearty, McBee Audio Labs 
 * ( www.mcbeeaudio.com )
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "initialise.h"
#include "scheduler.h"
#include "xprintf.h"
#include <xc.h>

// DEVCFG3
#pragma config FVBUSONIO = OFF          // USB VBUS ON Selection (Controlled by Port Function)

// DEVCFG2
// we have an 8MHz XTAL on the MAX32. This is how the clocking is worked out:
//  - the PLL input has to be from 4 to5MHz, so we divide by 2 to get 4MHz
//  - to get our Tri Sensor baud rate of 921.6 kbaud it turns out that PBclk = 48MHz is best (<1% err)
//  - CPU clock is maximum of 80MHz, so we use 48MHz for that too.
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_24         // PLL Multiplier (20x Multiplier)
#pragma config UPLLIDIV = DIV_12        // USB PLL Input Divider 
#pragma config UPLLEN = OFF             // USB PLL Enable (Disabled and Bypassed)
#pragma config FPLLODIV = DIV_2         // System PLL Output Clock Divider (PLL Divide by 1)

// DEVCFG1
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Primary Osc w/PLL (XT+,HS+,EC+PLL))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config IESO = ON                // Internal/External Switch Over (Enabled)
#pragma config POSCMOD = XT             // Primary Oscillator Configuration (XT osc mode)
#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_1           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/1)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Disabled (SWDTEN Bit Controls))

// DEVCFG0
#pragma config DEBUG = OFF              // Background Debugger Enable (Debugger is disabled)
#pragma config ICESEL = ICS_PGx2        // ICE/ICD Comm Channel Select (ICE EMUC2/EMUD2 pins shared with PGC2/PGD2)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

// --- EXECUTION ---
int32_t main(int32_t argc, char** argv) {
    
    __builtin_disable_interrupts();
    initialise();
    init_scheduler();
    xprintf("\r\nPICKIT MAX32 RT Scheduler\r\n");
    xprintf("=========================\r\n");
    __builtin_enable_interrupts();

    while(1){
      run_scheduler();
      while (!task_scheduler_flag) {
          debug_print_char();
      }
    }
}

