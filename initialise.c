/* 
 * File:   initialise.c
 * Project : Cooperative scheduler for Digilent MAX32
 * Author: Daniel McBrearty, McBee Audio Labs 
 * ( www.mcbeeaudio.com )
 *
 */

#include "initialise.h"
#include "debug_uart.h"

void initialise(void){
    
    // clock out
    OSCCONbits.COSC = 0b011; // pri osc with PLL
    
    //disable JTAG, interferes with RA0and RA1
    DDPCON = 0;
    
    // IO pins
    // first we just disable all the fancy stuff that we do not need
    AD1CON1bits.ON = 0; // analogue ins off
    AD1PCFG =0xFFFF;    // digital ins not analogue! (or port B won't work))
    ODCA = 0;           // no open col ops
    ODCB = 0;  
    CNCON = 0;          // no change notification
    CNEN = 0;
    CNPUE = 0;

    // now we set up the pins we actually want
    // pins are input by default on reset, so we only create outputs
    
    //first define the initial output value 
    RUN_LED = 0;
    
    // now set the ports as output

    TRISAbits.TRISA3 = 0;   // RUN LED
    
    // Timer 1 (Task Scheduler Ticks)
    T1CONbits.ON = 0;           // timer off
    T1CONbits.TCS = 0;          // use internal clock
    T1CONbits.TCKPS1 = 1;       // prescale by 64
    T1CONbits.TCKPS0 = 0;       //    => 1.3us @ 48MHz
    PR1 = SYSTEM_TICK_TIMER;    // System Timer tick is 5ms 
    T1CONbits.TGATE = 0;        // no gating
    TMR1 = 0; 
    T1CONbits.ON = 1;           // timer on
    
    // Timer 2 ( for demonstrating debug prints from an ISR)
    T2CONbits.ON = 0;           // timer off
    T2CONbits.TCS = 0;          // use internal clock
    T2CONbits.TCKPS1 = 1;       // prescale by 64
    T2CONbits.TCKPS0 = 0;       //    => 1.3us @ 48MHz
    PR2 = 1000;                   // 
    T2CONbits.TGATE = 0;        // no gating
    TMR2 = 0; 
    
    // U1 is for debug over USB serial for MAX32 board
    U1MODEbits.BRGH = 1;
    U1BRG = 12;              // 921.6kb
    U1MODEbits.STSEL = 0;   // 1 stop bit
    U1MODEbits.PDSEL0 = 0;  // 8 bits, no parity
    U1MODEbits.PDSEL1 = 0;
    U1STAbits.UTXEN = 1;
    U1STAbits.URXEN = 0;
    U1MODEbits.ON = 1;
    
    // --- Initialise INTERRUPTS ---
    INTCONbits.MVEC = 1; // multi vector
    INTCONbits.TPC = 0; // no proximity timer
    
    // T1 interrupt - priority 3 (Task Switcher))
    IPC1bits.T1IP = 3; // T1 - priority 3
    IPC1bits.T1IS = 0; // doesn't matter, no groups
    IFS0bits.T1IF = 0; // reset the flag
    IEC0bits.T1IE = 1; // enable ints for T1
    // T2 interrupt - priority 5 (debug print)
    IPC2bits.T2IP = 5; // T1 - priority 3
    IPC2bits.T2IS = 0; // doesn't matter, no groups
    IFS0bits.T2IF = 0; // reset the flag
    IEC0bits.T2IE = 1; // enable ints for T2
    
    init_debug_uart();
}

