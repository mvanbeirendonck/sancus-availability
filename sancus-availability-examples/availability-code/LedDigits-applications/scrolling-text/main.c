#include <sancus/sm_support.h>
#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include <sancus_support/uart.h>




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


void led_digits_init(void)
{
    LED1 = 0x71; // 'F'
    LED2 = 0x79; // 'E'
    LED3 = 0x79; // 'E'
    LED4 = 0x7c; // 'b'
    LED5 = 0x5e; // 'd'
    LED6 = 0x77; // 'A'
    LED7 = 0x79; // 'E'
    LED8 = 0x5E; // 'd'
    printf("LED1 is %x\n", LED1);
}


 
void mySleep(long sleepTime)
{
   long i;
   for(i=0; i<sleepTime;i++);
}

void write_led_digits(void)
{
    volatile uint8_t foo = LED1;
    LED1 = LED2;
    LED2 = LED3;
    LED3 = LED4;
    LED4 = LED5;
    LED5 = LED6;
    LED6 = LED7;
    LED7 = LED8;
    LED8 = foo;
    mySleep(200000);
}

int main()
{
    WDTCTL = WDTPW | WDTHOLD;
#ifndef __SANCUS_SIM
    uart_init();
#endif
    
    puts("\n------\n[main] hello world!");

    led_digits_init();
             
    while(1)
    {
    write_led_digits(); 
    }
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
