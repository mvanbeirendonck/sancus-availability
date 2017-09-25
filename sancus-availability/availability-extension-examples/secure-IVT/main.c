#include <sancus/sm_support.h>
#include <msp430f1121.h>
#include <stdio.h>
#include <sancus_support/uart.h>
#include <sancus/reactive_stubs_support.h>
#include "sm_io_wrap.h"


#define IVT_start (uint8_t*) 0xffe0
#define IVT_end (uint8_t*) 0xfffe

DECLARE_SM(IVT,0x1234);

void SM_ENTRY(IVT) IVT_entry(void) {
}

void timerA_init(void)
{
    printf("Initializing timerA\n");
    TACCTL0 = CCIE;
    TACTL = TASSEL_2 + MC_2; 
    TACCR0 = 100;
    printf("Interrupts enabled \n");
    asm("eint");
}

void __attribute__((__interrupt__ (TIMERA0_VECTOR))) Timer_A0(void)
{
    printf("TimerA interrupt received!\n");
    asm("dint");
}

void attacker_ISR(void) {
    while(1);
}

int main()
{
    WDTCTL = WDTPW + WDTHOLD;
    uart_init();
    
    printf("main started\n");

    IVT.public_start = IVT_start;
    IVT.public_end = IVT_end;
    sancus_enable(&IVT);
    dump_sm_layout(&IVT);

  
    timerA_init();

    

    
    int* vector;
    vector = (int*) IVT_start + TIMERA0_VECTOR;

    asm("mov.b %0, &0xfff2": : "rm"(&attacker_ISR)); // The IVT is now write protected
    while(1);
}

