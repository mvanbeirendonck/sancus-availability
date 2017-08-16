#include <sancus/sm_support.h>
#include <msp430f1121.h>
#include <stdio.h>
#include <sancus_support/uart.h>
#include <sancus/reactive_stubs_support.h>
#include "sm_io_wrap.h"
#include <string.h>




DECLARE_SM(dummy,0x1234);
DECLARE_SM(bar,0x1234);
DECLARE_SM(baz,0x1234);
DECLARE_SM(quux,0x1234);
DECLARE_SM(quuux,0x1234);


void SM_ENTRY(bar) bar_entry(void) {}
void SM_ENTRY(baz) baz_entry(void) {}
void SM_ENTRY(quux) quux_entry(void) {}
void SM_ENTRY(quuux) quuux_entry(void) {}

void print_bytes(const char* bytes, size_t n)
{
    int i;
    for (i = 0; i < n; i++)
        printf("%02x", bytes[i] & 0xff);
}

void SM_FUNC(dummy) print_bytes_secure(const char* bytes, size_t n)
{
    int i;
    for (i = 0; i < n; i++)
        printf1("%02x", bytes[i] & 0xff);
}

void SM_ENTRY(dummy) dummy_entry(void* tag, void* continuation) 
{
        int succes;

	char dummy_tag[SANCUS_TAG_SIZE];
	succes = sancus_tag(&continuation,sizeof(continuation),dummy_tag);

	printf1("succes: %d\n",succes);
        puts("MAC: ");
    	print_bytes_secure(dummy_tag, sizeof(dummy_tag));
    	puts("\n");

	//if (&dummy_tag == &tag) {
	//	sancus_disable(continuation);
	//}
}

void* SM_ENTRY(dummy) dummy_tag(void* data)
{
        void* tag = NULL;
	sancus_tag(data,sizeof(data),tag);
        return tag;
}


void quuux_enable(void)
{
	sancus_enable(&quuux);
}


int main()
{
    	WDTCTL = WDTPW + WDTHOLD;
    
    	printf("main started\n");
	sancus_enable(&dummy);
    	sancus_enable(&bar);
    	sancus_enable(&baz);
    	sancus_enable(&quux);
	//sancus_enable(&quuux); //ignored because only 4 SM supported


        char main_tag[SANCUS_TAG_SIZE];
        char key[SANCUS_KEY_SIZE];
        uint64_t key_value = 0xa41f6a1934160034; //TODO: use correct key, how?
        void* continuation = &quuux_enable;

	sprintf(key,"%" PRIu64 ,key_value); //TODO: verify that this copies key_value into key
	sancus_tag_with_key(key,
			    &continuation,sizeof(continuation),
			    main_tag);

	printf("MAC: ");
    	print_bytes(main_tag, sizeof(main_tag));
   	printf("\n");


	dummy_entry(main_tag, continuation);

	

   
}



