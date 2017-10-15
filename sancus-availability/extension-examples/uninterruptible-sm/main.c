#include <sancus/sm_support.h>
#include <msp430f1121.h>
#include <stdio.h>
#include <sancus_support/uart.h>
#include <sancus/reactive_stubs_support.h>
#include "sm_io_wrap.h"


// This example program shows that SM's are currently uninterruptible (install extensions!!)


DECLARE_SM(foo,0x1234);

void SM_ENTRY(foo) foo_print(void) {

    TACCTL0 = CCIE;
    TACCR0 = 100;
    TACTL = TASSEL_2 + MC_2; 
    asm("eint");
    //puts("going into infinite loop\n");
    while(1);
    
}



void __attribute__((__interrupt__ (TIMERA0_VECTOR))) Timer_A0(void)
{
    printf("[attacker] I interrupted [foo]\n");
}

int main()
{
    WDTCTL = WDTPW + WDTHOLD;
    uart_init();
    
    printf("main started\n");
    
    sancus_enable(&foo);
    foo_print();
}

