/* 
 * File:   scheduler.c
 * Project : Cooperative scheduler for Digilent MAX32
 * Author: Daniel McBrearty, McBee Audio Labs 
 * ( www.mcbeeaudio.com )
 *
 */

#include "scheduler.h"
#include "initialise.h"

#define NUM_TASKS 6
task tasks[NUM_TASKS]; // task list

void task_tick_counter(void);
void task_blink_on(void);
void task_blink_off(void);
void task_start_print_timer(void);
void task_print_two_secs(void);
void task_load_monitor(void);

void init_scheduler(void){
    // just inc the system counter
    tasks[0].elapsedTime = 0;
    tasks[0].period = 1; //
    tasks[0].TickFct = &task_tick_counter;
    // turn on blink LED (about every second))
    tasks[1].elapsedTime = 0;
    tasks[1].period = (SYSTEM_TICK_TIMER / 16 ); // just over 1s
    tasks[1].TickFct = &task_blink_on;
    // see if the LED should turn off
    tasks[2].elapsedTime = 0;
    tasks[2].period = 1; // 
    tasks[2].TickFct = &task_blink_off;
    // start T2 which will raise an int and print something, every 10 secs
    tasks[3].elapsedTime = 0;
    tasks[3].period = 2000;
    tasks[3].TickFct = &task_start_print_timer; 
    // just print a boring message every two secs
    tasks[4].elapsedTime = 0;
    tasks[4].period = 400;
    tasks[4].TickFct = &task_print_two_secs; 
    // monitor system worst case load (used to control blink LED)
    tasks[5].elapsedTime = 0;
    tasks[5].period = 1; 
    tasks[5].TickFct = &task_load_monitor; 
}

void run_scheduler(void){
    uint32_t  t;
    for (t = 0; t < NUM_TASKS; ++t) {
         if (tasks[t].elapsedTime >= tasks[t].period) { // Ready
            tasks[t].TickFct(); // Go
            tasks[t].elapsedTime = 0;
         }
         tasks[t].elapsedTime++;
      }
    task_scheduler_flag = 0;
}

// --- TASKS ---

void task_tick_counter(void){
    tick_counter++;
}

// we use the blink LED to give an idea of system load
// the period is about 1 second
// the pulse width shows the maximum amount of tick time we have used to
// complete all tasks

void task_blink_on(void){
    RUN_LED = 1;
    led_counter = 0;
}

void task_blink_off(void){
    uint32_t max_elapsed_time = system_timer_max;
    max_elapsed_time = max_elapsed_time >> 4; // divide by 16
    led_counter++;
    if (led_counter >= max_elapsed_time){
        RUN_LED = 0;
    }
}

void task_start_print_timer(void){
    /* just start timer 2 */
    TMR2 = 0; 
    T2CONbits.ON = 1;
}

void task_print_two_secs(void){
    xprintf("sys counter is : %d\r\n", tick_counter);
}

void task_load_monitor(void){
    /*
     * This should be the last task in the list. We see how close we are
     * to running out out of time in the task manager.
     */
    uint32_t sys_timer = TMR1;
    if (sys_timer > system_timer_max){
        system_timer_max = sys_timer;
    }
}

// --- UTILITY FUNCTIONS ---

void fatal_error(int8_t * msg, int32_t e ){
    /*
     * All errors are fatal. We turn on the LED and complain on the debug port
     */
    __builtin_disable_interrupts();
    int32_t i = 0;
    RUN_LED = 1;
    while(1){
        xprintf("\r\n!!! -- FATAL ERROR (%d) -- !!!\r\n", e);
        xprintf(msg);
        xprintf("\r\n");
        for (i = 0; i < 1000000;i++);
    }
}

// Timer TickISR - drives low level tasks

void __ISR(_TIMER_1_VECTOR, IPL3AUTO) Timer1Tick(void){
    /* TIMER 1 generates 5ms ticks for the Task Scheduler */
    if(task_scheduler_flag){ // fatal error, tasks did not complete
        fatal_error("Scheduler Overrun Error.", 0);
    } else {
        task_scheduler_flag = 1;
    }
    IFS0bits.T1IF = 0; // reset the flag
}

void __ISR(_TIMER_2_VECTOR, IPL5AUTO) Timer2Tick(void){
    /* TIMER 2 just prints a debug message and turns itself off (one-shot timer). 
     * This is just to demonstrate that debug prints can work from ISR's, but 
     * will corrupt debug strings prints from the task scheduler. */
    xprintf("*TIMER 2*");
    T2CONbits.ON = 0;           // timer off
    IFS0bits.T2IF = 0; // reset the flag
}