#include <msp430.h>
#include <stdio.h>
#include <sancus/sm_support.h>
#include <sancus_support/uart.h>
#include <sancus_support/sm_io.h>

// INSPECT THE GTKWAVE FILE TO VERIFY THAT THE MALICIOUS ISR GETS INTERRUPTED 

DECLARE_SM(foo,0x1234);
DECLARE_EXCLUSIVE_MMIO_SM(baz,0x0090,0x0097,1,0x1234); // Exclusive access to foo
SM_HANDLE_IRQ(foo,9);

unsigned int SM_DATA(foo) foo_var;

void __attribute__((__interrupt__ (0x001A))) sm_irq(void) // Attacker compromised violation ISR
{
    asm("dint"); // Won't work from unprotected code
    while(1)
    {
    printf("I have control!\n");
    }	
}


void SM_MMIO_ENTRY(baz) baz_entry(void)
{
}

void SM_ENTRY(foo) foo_entry(void)
{
}

SM_ISR(foo,num)
{ 
    puts("TimerA interrupt received!\n"); // Shows that nested interrupts are enabled
}


int main()
{
    WDTCTL = WDTPW + WDTHOLD;
    uart_init();

    puts("\n------\n\n");
    printf("main started\n");

    sancus_enable(&foo);
    sancus_enable(&baz);

    TACCTL0 = CCIE;
    TACTL = TASSEL_2 + MC_2 + ID_3;
    TACCR0 = 300;
    asm("eint");
    
    baz_entry();
    while(1);
}





