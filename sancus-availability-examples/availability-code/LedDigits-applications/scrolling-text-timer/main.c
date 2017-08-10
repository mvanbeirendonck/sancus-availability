#include <sancus/sm_support.h>
#include <msp430f1121.h>
#include <stdio.h>
#include <stdlib.h>
#include <sancus_support/uart.h>
#include <string.h>
//#include <time.h>

// Implementation of scrolling text through Timer_A interrupts

// MMIO registers for led digit segments:
// (MSB-1) g f e d c b a (LSB)
#define LED1 *(volatile unsigned char*) 0x0090 //LED1 is the value at 0x0090
#define LED2 *(volatile unsigned char*) 0x0091
#define LED3 *(volatile unsigned char*) 0x0092
#define LED4 *(volatile unsigned char*) 0x0093
#define LED5 *(volatile unsigned char*) 0x0094
#define LED6 *(volatile unsigned char*) 0x0095
#define LED7 *(volatile unsigned char*) 0x0096
#define LED8 *(volatile unsigned char*) 0x0097
#define _bis_SR_register(x) \
    __asm__ __volatile__ ("bis.w %0, r2" \
        : : "ri"((unsigned int) x) \
    )


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
}



void write_led_digits(void)
{
    volatile unsigned char foo = LED1;
    LED1 = LED2;
    LED2 = LED3;
    LED3 = LED4;
    LED4 = LED5;
    LED5 = LED6;
    LED6 = LED7;
    LED7 = LED8;
    LED8 = foo;
}



unsigned int timerCount=0;

int main()
{
    printf("main started \n");
    WDTCTL = WDTPW + WDTHOLD;

#ifndef __SANCUS_SIM
    uart_init();
#endif
    
    TACCTL0 = CCIE;
    TACTL = TASSEL_1 + MC_2; // TASSEL_1 doesn't work
    TACCR0 = 800;

    led_digits_init();                       
    _bis_SR_register(GIE); // LPM0 w/ interrupt
    while(1);

}



void __attribute__((__interrupt__ (TIMERA0_VECTOR))) Timer_A0(void)
{
  printf("CYCLYING LEDS\n");
  write_led_digits(); 
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
