#include <sancus/sm_support.h>
#include <msp430.h>
#include <stdio.h>

// Test to produce a module's key.


void SM_ENTRY(foo) hello_world(void)
{
    puts("Hello World");
}

DECLARE_SM(foo,0x1234);

int main()
{
    WDTCTL = WDTPW | WDTHOLD;

    puts("main started");

    sancus_enable(&foo);
    hello_world();
    puts("main done");
    
	
}


int putchar(int c)
{
    P1OUT = c;
    P1OUT |= 0x80;
    return c;
}
