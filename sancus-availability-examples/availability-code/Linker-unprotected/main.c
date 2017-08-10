#include <sancus/sm_support.h>
#include <msp430.h>
#include <stdio.h>

// Test to call some unprotected code from a SM

DECLARE_SM(foo,0x1234);

void another_unprotected_func(void) {

    // printf is not included in the Calls unprotected list which
    // is okay as it's only called from another unprotected function.
    printf("another unprotected function!");
}


void SM_FUNC(foo) some_func(void)
{
    another_unprotected_func();
}


void some_unprotected_func(void) {
}

void SM_ENTRY(foo) hello_world(void)
{
    puts("hello world!");
    some_func();
    some_unprotected_func();

    
}




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
