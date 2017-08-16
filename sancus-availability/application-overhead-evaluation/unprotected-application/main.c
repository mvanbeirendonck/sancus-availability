#include <sancus/sm_support.h>
#include <msp430f1121.h>
#include <stdio.h>
#include <sancus_support/uart.h>
#include <sancus/reactive_stubs_support.h>
#include <sancus_support/tsc.h>

// MMIO registers for led digit segments:
// (MSB-1) g f e d c b a (LSB)
#define LED1 *(volatile uint8_t*) 0x0090 //LED1 is the value at 0x0090
#define LED2 *(volatile uint8_t*) 0x0091
#define LED3 *(volatile uint8_t*) 0x0092
#define LED4 *(volatile uint8_t*) 0x0093
#define LED5 *(volatile uint8_t*) 0x0094
#define LED6 *(volatile uint8_t*) 0x0095
#define LED7 *(volatile uint8_t*) 0x0096
#define LED8 *(volatile uint8_t*) 0x0097

DECLARE_TSC_TIMER(overhead_timer);

void led_init(void)
{
    LED1 = 0x71; // 'F'
    LED2 = 0x79; // 'E'
    LED3 = 0x79; // 'E'
    LED4 = 0x7c; // 'b'
    LED5 = 0x5e; // 'd'
    LED6 = 0x77; // 'A'
    LED7 = 0x79; // 'E'
    LED8 = 0x5E; // 'd'
    TACCTL0 = CCIE;
    TACTL = TASSEL_2 + MC_1;
    TACCR0 = 1;
}



void cycle_leds(void)
{
    uint8_t foo = LED1;
    LED1 = LED2;
    LED2 = LED3;
    LED3 = LED4;
    LED4 = LED5;
    LED5 = LED6;
    LED6 = LED7;
    LED7 = LED8;
    LED8 = foo;
}

void __attribute__((__interrupt__ (TIMERA0_VECTOR))) Timer_A0(void)
{
    cycle_leds();
}


int main()
{
    WDTCTL = WDTPW + WDTHOLD;
    uart_init();

    printf("main started\n");

    led_init();      
                
    //TODO: check if printing in simulation environement doesn't induce extra overhead.
    TSC_TIMER_START(overhead_timer);
    asm("eint");
    asm("dint");
    TSC_TIMER_END(overhead_timer); 
    //result = interval - 6 = 121 - 6 = 115
    // 6 = eint + dint + 4 cycles tsc timer

    while(1);
}






int putchar(int c)
{
#if __SANCUS_SIM
    // simulator
    P1OUT = c;
    P1OUT |= 0x80;
#else
    // uart
    if (c == '\n')
        putchar('\r');

    uart_write_byte(c);
#endif
    return c;
}
