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

DECLARE_SM(foo,0x1234);

void some_func(void) {
    int i = 1;
}

void SM_ENTRY(foo) foo_entry(void) {
    
    some_func();
    int x = 5;
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

    foo_entry();
}

