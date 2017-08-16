#include <sancus/sm_support.h>
#include <msp430f1121.h>
#include <stdio.h>
#include <sancus_support/uart.h>
#include <sancus/reactive_stubs_support.h>
#include "sm_io_wrap.h"

#define _bis_SR_register(x) \
    __asm__ __volatile__ ("bis.w %0, r2" \
        : : "ri"((unsigned int) x) \
    )

DECLARE_MMIO_SM(IVT,0xffe0,0xfffe,0x1234);

void timerA_init(void)
{
    printf("Initializing timerA\n");
    TACCTL0 = CCIE;
    TACTL = TASSEL_2 + MC_2; 
    TACCR0 = 1000;
    _bis_SR_register(GIE);
    printf("Interrupts enabled \n");
}

void __attribute__((__interrupt__ (TIMERA0_VECTOR))) Timer_A0(void)
{
    printf("TimerA interrupt received!");
}

int main()
{
    WDTCTL = WDTPW + WDTHOLD;
    
    printf("main started\n");
    sancus_enable(&IVT);

  
    timerA_init();

    
#ifndef __SANCUS_SIM
    uart_init();
#endif
    
    while(1);
}

// Findings: how the CPU retrieves its value from the IVT seems to bypass Sancus memory protection.
// --> There are seperate memory address busses for the frontend and the execution unit.
// --> Violations only register when eu_mb_en, a read in the IVT goes through fe_mb_en.

