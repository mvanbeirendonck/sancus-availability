#include <sancus/sm_support.h>
#include <msp430f1121.h>
#include <stdio.h>
#include <sancus_support/uart.h>
#include <sancus/reactive_stubs_support.h>
#include "sm_io_wrap.h"


DECLARE_SM(foo,0x1234);
DECLARE_SM(bar,0x1234);

#define _bis_SR_register(x) \
    __asm__ __volatile__ ("bis.w %0, r2" \
        : : "ri"((unsigned int) x) \
    )

const int SM_DATA(foo) foo_const = 0xcafe;
volatile int SM_DATA(foo) foo_var;

void SM_NAKED_ENTRY(bar) entry_reserved_for_foo(void)
{
  //asm("clr &__sm_bar_entry");
}			  
		 
void SM_ENTRY(foo) some_entry(void)
{
  printf0("[foo] Hey there.\n");
  foo_var = 0xbeef;

  //asm("clr &__sm_foo_entry");

  
  //printf0("[foo] Calling bar.\n");
  //entry_reserved_for_foo();
}

void SM_FUNC(foo) some_non_entry(void)
{
  printf0("[foo] This entry is off limits!\n");
}


void __attribute__((__interrupt__ (0x001A))) sm_irq(void)
{
  printf("Detected SM violation!\n");
}


int main()
{
    printf("main started\n");
    WDTCTL = WDTPW + WDTHOLD;
    
    _bis_SR_register(GIE);

    sancus_enable(&foo);
    dump_sm_layout(&foo);
    sancus_enable(&bar);
    dump_sm_layout(&bar);

    some_entry();

    
#ifndef __SANCUS_SIM
    uart_init();
#endif

    
    struct SancusModule newModule;
    newModule.public_start =(int*) 0x0092;
    newModule.public_end =(int*) 0x00FF;

    printf("Exec violations\n---------------------\n");
    //entry_reserved_for_foo(); // Exec violation
    asm("call %0": : "r"(foo.public_start+0x2)); // Exec violation
    printf("Mem violations\n---------------------\n");
    printf("First one:\n");
    printf("[main] foo secret constant is: %d\n",foo_const); // Mem violation
    printf("Second one:\n");
    asm("mov.b #16, &0x0090"); // Mem violation
    printf("Third one:\n");
    printf("[main] foo secret variable is: %d\n",foo_var);
    printf("Create violation:\n---------------------\n");
    sancus_enable(&newModule); // Create violation
    printf("main finished\n");


    // Findings: create_violations don't go to IRQ13 because SANCUS_ENABLE is being executed
    // Reading a constant is not a memory violation because of compiler optimizations
    // Unauthorized calls to NAKED_SM entry produce mem_violation instead of exec_violations
}

