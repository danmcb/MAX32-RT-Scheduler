/* 
 * File:   initialise.h
 * Project : Cooperative scheduler for Digilent MAX32
 * Author: Daniel McBrearty, McBee Audio Labs 
 * ( www.mcbeeaudio.com )
 *
 */

#ifndef INITIALISE_H
#define	INITIALISE_H

#include <xc.h>
#include <sys/attribs.h>

// outputs
// the on board green LED
#define RUN_LED LATAbits.LATA3
// pin 22 on the MAX32 - just for verifying timing with scope.
// You can remove it if you want.
#define DEBUG_PIN LATCbits.LATC2

// value of PR1, system tick timer = 5ms @ 48MHz Timer 1 prescaler = 64
#define SYSTEM_TICK_TIMER 3750

void initialise(void);

#endif	/* INITIALISE_H */

