#include <sancus/sm_support.h>
#include <msp430.h>
#include <stdio.h>

DECLARE_SM(bar,1234);
DECLARE_SM(foo,1234);
DECLARE_SM(michiel,123);

sm_id SM_ENTRY(foo) get_foo_id(void)
{
    return sancus_get_self_id();
}

void SM_ENTRY(foo) foo_entry(void)
{
    puts("This is another entry point for foo");
}

sm_id SM_ENTRY(bar) get_bar_id(void)
{
    return sancus_get_self_id();
}

sm_id SM_ENTRY(michiel) get_michiel_id(void)
{
    return sancus_get_self_id();
}


int main()
{
    WDTCTL = WDTPW | WDTHOLD;

    sancus_enable(&foo);
    sancus_enable(&bar);
    sancus_enable(&michiel);

    printf("----------MODULE ID----------\n");
    printf("foo id: %d\n",get_foo_id());
    printf("bar id: %d\n",get_bar_id());
    printf("michiel id: %d\n",get_michiel_id());

    printf("michiel id: %d\n", sancus_get_id(michiel.public_start));

    printf("----------ENTRY POINTS----------\n");
    printf("foo entry point: %p\n",SM_GET_ENTRY(foo));
    printf("get_foo_id entry index: %d\n", SM_GET_ENTRY_IDX(foo,get_foo_id));
    printf("foo_entry entry index: %d\n", SM_GET_ENTRY_IDX(foo,foo_entry));

    printf("----------TAGS,NONCE,ID----------\n");
    //printf("GET TAG: caller foo, callee bar: %p\n",SM_GET_TAG(foo,bar));
    //printf("GET VERIFY ID: caller foo, callee bar: %d\n",*SM_GET_VERIFY_ID(foo,bar));
    //printf("GET WRAP NONCE: foo: %u\n",SM_GET_WRAP_NONCE(foo));
    //printf("GET WRAP TAG: foo: %p\n",SM_GET_WRAP_TAG(foo));
    

  

	
}


int putchar(int c)
{
    P1OUT = c;
    P1OUT |= 0x80;
    return c;
}
