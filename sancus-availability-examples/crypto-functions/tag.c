#include <sancus/sm_support.h>

#include <msp430.h>
#include <stdio.h>

// Test to produce a module's key.
// Test to produce a tag over some data.

void print_bytes(const char* bytes, size_t n)
{
    int i;
    for (i = 0; i < n; i++)
        printf("%02x", bytes[i] & 0xff);
}

void* SM_ENTRY(foo) get_tag(char* data)
{
    void* tag=NULL;
    puts("computing tag");
    sancus_tag(data,sizeof(data),tag); // this calls main() and results in an infinite loop. Memory violation?
    puts("returning tag");
    return tag;

}

DECLARE_SM(foo,1234);

int main()
{
    WDTCTL = WDTPW | WDTHOLD;

    puts("main started");

    sancus_enable(&foo);

    void* tag = get_tag("abcd");
    puts("received tag");

    printf(", MAC: ");
    print_bytes(tag, sizeof(tag));
    printf("\n");

    
    puts("main done");
    
	
}


int putchar(int c)
{
    P1OUT = c;
    P1OUT |= 0x80;
    return c;
}
