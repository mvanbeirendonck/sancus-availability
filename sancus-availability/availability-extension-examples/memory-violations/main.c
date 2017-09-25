#include <sancus/sm_support.h>
#include <msp430f1121.h>
#include <stdio.h>
#include <sancus_support/uart.h>
#include <sancus/reactive_stubs_support.h>
#include "sm_io_wrap.h"
#include <sancus_support/sm_io.h>



DECLARE_SM(foo,0x1234);
DECLARE_SM(bar,0x4321);
DECLARE_EXCLUSIVE_MMIO_SM(baz,0x0090,0x0097,1,0x1234); // Exclusive access to foo

unsigned int SM_DATA(foo) foo_var;

void __attribute__((__interrupt__ (0x001A))) sm_irq(void)
{
    while(1);
}

void SM_ENTRY(bar) bar_entry(void)
{
}

void SM_MMIO_ENTRY(baz) baz_entry(void)
{
}

void SM_ENTRY(foo) foo_entry(void)
{
    foo_var = 0xfeca;
}

unsigned int SM_ENTRY(foo) foo_read(unsigned int* address)
{
    return *address;
}

void SM_FUNC(foo) foo_private(void)
{
    while(1);
}

int main()
{
    WDTCTL = WDTPW + WDTHOLD;
#ifndef __SANCUS_SIM
    uart_init();
#endif
    
    printf("main started\n");

    unsigned int* foo_entry_add = (unsigned int*) &foo_entry;
    //printf("Foo entry address: %p\n",foo_entry_add);
    //printf("Foo entry address contains: 0x%04x\n",*foo_entry_add);

    sancus_enable(&foo);
    dump_sm_layout(&foo);
    sancus_enable(&baz);
    dump_sm_layout(&baz);

    unsigned int violation = 5;

    printf("----------------------------\n");
    printf("Memory violations\n");
    printf("----------------------------\n");

    printf("Public read violation\n");
    violation = *foo_entry_add; 	// Public read violation
    ASSERT(violation != foo_read(foo_entry_add));
    //printf("0x%04x\n",violation);

    printf("Public write violation\n");
    *foo_entry_add = 0xcafe; 		// Public write violation
    ASSERT(foo_read(foo_entry_add) != 0xcafe);
    //printf("0x%04x\n",foo_read(foo_entry_add));

    printf("Private read violation\n");
    foo_entry();
    violation = 5;
    violation =  foo_var; 		// Private read violation
    ASSERT(violation != 0xfeca);
    //printf("0x%04x\n",foo_read(&foo_var));
    //printf("0x%04x\n",violation);

    printf("Private write violation\n");
    foo_var = 0xcafe;			// Private write violation
    ASSERT(foo_read(&foo_var) != 0xcafe);
    //printf("0x%04x\n",foo_read(&foo_var));

    //printf("----------------------------\n");
    //printf("SM create violations\n");
    //printf("----------------------------\n");

    //printf("Public overlaps with public\n");
    //bar.public_start = foo.public_start;
    //bar.public_end = foo.public_end;
    //sancus_enable(&bar);

    //printf("Public overlaps with private\n");
    //bar.public_start = foo.secret_start;
    //bar.public_end = foo.secret_end;
    //sancus_enable(&bar);

    //printf("Private overlaps with public\n");
    //bar.public_start = foo.secret_end + 2;
    //bar.public_end = foo.secret_end + 3000;
    //bar.secret_start = foo.public_start;
    //bar.secret_end = foo.public_end;
    //sancus_enable(&bar);

    //printf("Private overlaps with private\n");
    //bar.secret_start = foo.secret_start;
    //bar.secret_end = foo.secret_end;
    //sancus_enable(&bar);
    //printf("----------------------------\n");

    printf("----------------------------\n");
    printf("Execute violations\n");
    printf("----------------------------\n");

    //printf("Outcall to private function \n");
    //asm("call %0": : "rm"(&foo_private));

    //printf("Illegal branch to text section \n");
    //asm("br %0": : "rm"(foo.public_start+0x2));

    printf("Outcall to exclusive driver \n");
    baz_entry();

    
    
  
    
}


