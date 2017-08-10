#include <sancus/sm_support.h>

#include <msp430.h>
#include <stdio.h>



DECLARE_SM(foo,1234);

void SM_ENTRY(foo) unprotect_foo(void)
{
    	sancus_disable();
}


int main()
{
    WDTCTL = WDTPW | WDTHOLD;

    puts("main started\n");
    sancus_enable(&foo);
    puts("foo enabled\n");
    unprotect_foo();
    puts("foo disabled\n"); // main never gets here
    puts("main finished\n");
   	
}


int putchar(int c)
{
    P1OUT = c;
    P1OUT |= 0x80;
    return c;
}
