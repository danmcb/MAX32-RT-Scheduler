/* 
 * File:   scheduler.c
 * Project : Cooperative scheduler for Digilent MAX32
 * Author: Daniel McBrearty, McBee Audio Labs 
 * ( www.mcbeeaudio.com )
 *
 */

#include "scheduler.h"
#include "initialise.h"
#include "debug_uart.h"

#define INCLUDE_TEST_TASKS

#ifdef INCLUDE_TEST_TASKS
#define NUM_TASKS 6
#else
#define NUM_TASKS 4
#endif

typedef struct task {
   uint32_t  period;         // Rate at which the task should tick
   uint32_t  elapsedTime;    // Time since task's last tick
   void (*TickFct)(void);       // Function to call for task's tick
} task;

volatile uint32_t task_scheduler_flag = 0;
uint32_t system_timer_max = 100; // we make this>0 so some LED flash is always visible
uint32_t led_counter = 0;
uint32_t tick_counter = 0;

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
    
#ifdef INCLUDE_TEST_TASKS
    // these tasks are for test/demo purposes. You can remove them if
    // not needed.
    
    // Start T2 which will raise an int and print something, every 10 secs
    tasks[3].elapsedTime = 0;
    tasks[3].period = 400;
    tasks[3].TickFct = &task_start_print_timer; 
    // just print a boring message every two secs
    tasks[4].elapsedTime = 0;
    tasks[4].period = 200;
    tasks[4].TickFct = &task_print_two_secs; 
#endif
    
    // monitor system worst case load (used to control blink LED)
    // this should run last
    tasks[NUM_TASKS - 1].elapsedTime = 0;
    tasks[NUM_TASKS - 1].period = 1; 
    tasks[NUM_TASKS - 1].TickFct = &task_load_monitor; 
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

uint32_t timer_tick(void){
    return task_scheduler_flag;
}

// --- TASKS ---

void task_tick_counter(void){
    //DEBUG_PIN = 1;
    tick_counter++;
    //DEBUG_PIN = 0;
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
        for (i = 0; i < 1000000;i++){
            debug_print_char();
        }
    }
}

// Timer TickISR - drives low level tasks

void __ISR(_TIMER_1_VECTOR, IPL1AUTO) Timer1Tick(void){
    /* TIMER 1 generates 5ms ticks for the Task Scheduler */
    if(task_scheduler_flag){ // fatal error, tasks did not complete
        fatal_error("Scheduler Overrun Error.", 0);
    } else {
        task_scheduler_flag = 1;
    }
    IFS0bits.T1IF = 0; // reset the flag
}

#ifdef INCLUDE_TEST_TASKS
// TASKS and ISR for test / demo purposes

void task_start_print_timer(void){
    /* This task starts Timer 2 which times out and raises an interrupt. The 
     * timeout interval is about 35us to 200us to interrupt the debug print from  
     * the next task_print_two_secs(). Hence we can test that the debug print
     * can cope with writes from different interrupt priorities.  */
    static uint32_t timeout_interval = 20;
    T2CONbits.ON = 0;
    TMR2 = 0; 
    PR2 = timeout_interval;
    timeout_interval += 3;
    if (timeout_interval > 150){
        timeout_interval = 20;
        // while(1); // uncomment to show error handling after 40 ticks!
    }
    T2CONbits.ON = 1;
    DEBUG_PIN = 1;
}

void task_print_two_secs(void){
    /* Here we just print a string to debug terminal. */
    xprintf("=============%d\r\n", tick_counter);
}

void __ISR(_TIMER_2_VECTOR, IPL2AUTO) Timer2Tick(void){
    /* TIMER 2 prints a debug count and turns itself off (one-shot timer). 
     * This is to prove that debug prints can work from ISR's. The count is 
     * so we can see if a print was missed. */
    static uint32_t count = 0;
    DEBUG_PIN = 0;
    xprintf("*T%d*", count);
    T2CONbits.ON = 0;           // timer off
    count++;
    IFS0bits.T2IF = 0; // reset the flag
}
#endif