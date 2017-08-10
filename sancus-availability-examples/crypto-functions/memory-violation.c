#include <sancus/sm_support.h>

#include <msp430.h>
#include <stdio.h>

DECLARE_SM(foo,1234);

void SM_ENTRY(foo) memory_violation2() // ALSO MEMORY-VIOLATION: A protected module cannot directly read it's secret section
{
    	char *p = foo.secret_start;
    	puts(p); 
}

void SM_ENTRY(foo) unprotect_foo()
{
    	sancus_disable();
}

void memory_violation() // MEMORY-VIOLATION 
{
    	char *p = foo.secret_start;
    	printf("first byte: %c\n",p[0]); 
}


int main()
{
    WDTCTL = WDTPW | WDTHOLD;

    sancus_enable(&foo);

    //printf("sm_id: %d\n",foo.id);
    //printf("vendor_id: %d\n",foo.vendor_id);
    //printf("name: %c\n",*foo.name);
    //printf("public_start: %p\n",foo.public_start);
    //printf("public_end: %p\n",foo.public_end);
    //printf("secret_start: %p\n",foo.secret_start);
    //printf("secret_end: %p\n",foo.secret_end);

    
    unprotect_foo();
    printf("unprotected\n"); // main never gets here, main is rerun after unprotect_foo()
    // also exec violation 176c -> 8bb6 is displayed after a key is again computed

    printf("secret_start: %p\n",foo.secret_start);
    char *p = foo.secret_start;
    printf("first byte: %c\n",p[0]); 	

    printf("Finished\n");
    

	
}


int putchar(int c)
{
    P1OUT = c;
    P1OUT |= 0x80;
    return c;
}
