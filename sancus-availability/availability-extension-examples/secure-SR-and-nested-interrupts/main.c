#include <msp430.h>
#include <stdio.h>
#include <sancus/sm_support.h>
#include <sancus_support/uart.h>
#include <sancus_support/sm_io.h>

// INSPECT THE GTKWAVE FILE TO VERIFY THAT THE MALICIOUS ISR GETS INTERRUPTED

DECLARE_SM(timerA_app,0x1234);
SM_HANDLE_IRQ(timerA_app,9);

unsigned int SM_DATA(timerA_app) secret_var = 0xcafe;


void SM_ENTRY(timerA_app) timerA_init(void)
{
    TACCTL0 = CCIE;
    TACTL = TASSEL_2 + MC_2 + ID_3;
    TACCR0 = 300;
    asm("eint");
}

SM_ISR(timerA_app,num)
{ 
    //puts("Interrupted the malicious ISR!\n");
}


void __attribute__((__interrupt__ (0x001A))) sm_irq(void) // Malicious ISR
{
    asm("dint"); //ISR tries to disable interrupts
    while(1);
}


int main()
{
    
    WDTCTL = WDTPW | WDTHOLD;
    uart_init();
    puts("\n------\n\n");
    printf("main started\n");
    sancus_enable(&timerA_app);

    timerA_init();
    unsigned int attacker_var;
    attacker_var = secret_var; //issue memory violation to invoke malicious ISR
    while(1);
}


