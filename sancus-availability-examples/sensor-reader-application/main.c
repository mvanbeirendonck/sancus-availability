//#include <sancus_support/uart.h>
#include <sancus/sm_support.h>
#include <msp430.h>
#include <stdio.h>
#include "reader.h"

// one protected module providing sensor data and one that transforms this data and signs it to be sent to the vendor.


ReaderOutput out;

void print_bytes(const char* bytes, size_t n)
{
    int i;
    for (i = 0; i < n; i++)
        printf("%02x", bytes[i] & 0xff);
}


int main()
{
    WDTCTL = WDTPW | WDTHOLD;

    printf("main started\n");

    sancus_enable(&sensor);
    sancus_enable(&reader);

    

    get_readings(&out);

    printf("Data: ");
    print_bytes((char*)&out.data, sizeof(out.data));
    printf(", MAC: ");
    print_bytes(out.mac, sizeof(out.mac));
    printf("\n");

    printf("main finished\n");

    return 0;
    
}



int putchar(int c)
{
    P1OUT = c;
    P1OUT |= 0x80;
    return c;
}
