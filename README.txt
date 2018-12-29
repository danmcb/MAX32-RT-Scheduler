Cooperative scheduler for Digilent MAX32 V1.0
=============================================

By: Daniel McBrearty, McBee Audio Labs ( www.mcbeeaudio.com ), McBee Tech ( http://www.mcbeetech.com ).

This is a simple, cooperative real time scheduler that runs on the Digilent MAX32 dev board. It can be built and loaded with the usual free Microchip toolchain (I use MPLAB X IDE v4.20 and XC32 v2.10 at time of writing).

This is designed to be as simple as possible, while still giving you all you need to build a hard real time system. It can be modified and hacked without too much hassle. It DWINTDANM (Does What It Needs To Do And No More).

Features:

SYSTEM
 - runs on Digilent MAX32 at 48MHz (PIC32MX795, 512K Flash, 64k RAM, 80MHz max.)
 - uses < 1% of memory resources.
 - only one UART, one timer and lowest interrupt level used. 
 - remaining IO, interrupts available for your system. (Five UARTS/SPI/I2C, four counters, ADC's ...)
 - easy to change system timing (scheduler rate, clock frequency, etc) to taste. Basic operation unchanged.
 - all C, no asm. Builds with free Microchip tools.

SCHEDULER:
 - cooperative scheduler runs at 5ms intervals. 
 - All tasks must complete in 5ms or a fatal error results.
 - Run LED shows scheduler is running (flashes about once per second). 
 - Run LED mark/space interval shows worst case scheduler load.

DEBUG PRINT:
 - buffered debug print functionality over builtin USB serial (912600baud, no serial converter needed)
 - formatted printing from within tasks or interrupts is fast (just writes to buffer)
 - scheduler dead time is used to feed UART with chars from buffer
 - can write to buffer from scheduler tasks OR higher level ISR's (message strings mix but no chars lost)
 - uses open source xprintf() from http://elm-chan.org/fsw/strf/xprintf.html (no f.p. support)

ERROR HANDLING
 - simple, brutal handler : kills scheduler, turns run LED on, and writes debug msg about every 5s.
 - use liberally : All Errors Are Fatal makes you find and fix software faults.

TEST/DEMO
 - demo tasks show basic functionality to debug terminal.
 - test code uses one timer and int level.
 - #define INCLUDE_TEST_TASKS in scheduler.c to build them (or not).
 - for educational purposes : remove #define CRITICAL_SECTION_SYNC to show errors in test debug output.

CREDITS:
 - Originally inspired by RIOS (http://www.cs.ucr.edu/~vahid/rios/), uses pretty much the same scheduler.
 - xprintf() thanks to the excellent Chan (http://elm-chan.org/fsw/strf/xprintf.html)

LICENSING:
This software is distributed under MIT License.
In addition - you are free to use this software for any purpose you want, as long as it is a nice one. (Hurting people, animals or the planet we share is not nice.)
Crediting and linking to mcbeeaudio.com and/or mcbeetech.com is appreciated.

THE USUAL DISCLAIMER AND CAVEAT (sigh):
This is free software. I make no guarantee of fitness for any purpose whatsoever. You use this software entirely at your own risk.
