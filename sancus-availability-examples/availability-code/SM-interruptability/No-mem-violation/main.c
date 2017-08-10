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

// This example program shows that SM's are currently interruptible when running a subroutine.
// No memory violation is called this time.


DECLARE_SM(foo,0x1234);

void SM_ENTRY(foo) foo_print(void) {
    
    
    while(1)
    {    
    printf0("[foo] I'm currently running\n"); 
    }
}


unsigned int timerCount;


#if __SANCUS_SIM
unsigned int timerMax = 2;
#else
unsigned int timerMax = 200;
#endif

void __attribute__((__interrupt__ (TIMERA0_VECTOR))) Timer_A0(void)
{
    printf("[attacker] I interrupted [foo]\n");
    timerCount = timerCount + 1;
    while(timerCount > timerMax)
    {
    printf("[attacker] I interrupted [foo] permanently and availability is gone\n");
    }
}

int main()
{
    WDTCTL = WDTPW + WDTHOLD;
    
    printf("main started\n");
    
    sancus_enable(&foo);
    dump_sm_layout(&foo);
     
#ifndef __SANCUS_SIM
    uart_init();
#endif
    
    TACCTL0 = CCIE;
    TACTL = TASSEL_2 + MC_2; 
    
#if __SANCUS_SIM
    TACCR0 = 5;
#else
    TACCR0 = 62500;
#endif
    _bis_SR_register(GIE);
    foo_print();
}

