#include <sancus/sm_support.h>
#include <msp430f1121.h>
#include <stdio.h>
#include <sancus_support/uart.h>
#include <sancus/reactive_stubs_support.h>
#include "sm_io_wrap.h"


DECLARE_SM(foo,0x1234);
SM_DATA(foo) int timerCount;
void SM_ENTRY(foo) foo_mod(void)
{
    timerCount = (timerCount + 1) % 3; // Mem violation if number after modulo > 2
				       // Men violation for any number if you assign it to a 					       // timerMax variable
    printf1("Timer is : %d \n",timerCount);
}

int main()
{
    WDTCTL = WDTPW + WDTHOLD;

#ifndef __SANCUS_SIM
    uart_init();
#endif
    
    


    printf("main started\n"); 
    sancus_enable(&foo);
    dump_sm_layout(&foo);
     

    while(1)
{
    foo_mod();
}
}

