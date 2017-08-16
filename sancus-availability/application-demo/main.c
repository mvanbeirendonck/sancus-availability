#include <sancus/sm_support.h>
#include <msp430f1121.h>
#include <stdio.h>
#include <sancus_support/uart.h>
#include <sancus/reactive_stubs_support.h>
#include "sm_io_wrap.h"

DECLARE_SM(led_app,0x1234);
DECLARE_EXCLUSIVE_MMIO_SM(led_driver,0x0090,0x0097,1,0x1234);
DECLARE_EXCLUSIVE_MMIO_SM(timerA_driver,0x012E,0x0176,1,0x1234);
SM_HANDLE_IRQ(led_app,9);
DECLARE_MMIO_SM(IVT,0xffe0,0xfffe,0x1234);


void SM_MMIO_ENTRY(led_driver) write_LED(uint8_t data, uint8_t led)
{
    asm ("add.b (%0), r14\n\t"
         "mov.b r15, -1(r14)\n\t"
         : 
         :"m" (led_driver.secret_start));
}

uint8_t SM_MMIO_ENTRY(led_driver) read_LED(uint8_t led)
{
    asm("add.b (%0),r15\n\t"
        "mov.b -1(r15),r15\n\t"        
        :
        :"m" (led_driver.secret_start));
}

void SM_MMIO_ENTRY(timerA_driver) write_timerA(uint16_t data, uint16_t address)
{
    asm("mov.w r15, @r14");
}

void SM_FUNC(led_app) timerA_init(void)
{
    write_timerA(CCIE,TACCTL0_);
    write_timerA(TASSEL_2 + MC_2 + ID_3, TACTL_);
    write_timerA(62500,TACCR0_);
}

SM_DATA(led_app) int ledInit=0;

void SM_ENTRY(led_app) led_init(void)
{
    if (ledInit==0)
    {
    	write_LED(0x71,0x1); // 'F'
    	write_LED(0x79,0x2); // 'E'
    	write_LED(0x79,0x3); // 'E'
    	write_LED(0x7c,0x4); // 'b'
    	write_LED(0x5e,0x5); // 'd'
    	write_LED(0x77,0x6); // 'A'
    	write_LED(0x79,0x7); // 'E'
    	write_LED(0x5E,0x8); // 'd'
        timerA_init(); 
	ledInit = 1;
    }
}

void SM_FUNC(led_app) cycle_leds(void)
{
    uint8_t foo = read_LED(0x1);
    write_LED(read_LED(0x2),0x1);
    write_LED(read_LED(0x3),0x2);
    write_LED(read_LED(0x4),0x3);
    write_LED(read_LED(0x5),0x4);
    write_LED(read_LED(0x6),0x5);
    write_LED(read_LED(0x7),0x6);
    write_LED(read_LED(0x8),0x7);
    write_LED(foo,0x8);
}

SM_DATA(led_app) int timerCount=0;


SM_ISR(led_app,num)
{ 
    timerCount = (timerCount+1)%40;
    if (timerCount == 0)
    {
        cycle_leds();
    }

}

int main()
{
    WDTCTL = WDTPW + WDTHOLD;
    uart_init();

    printf("main started\n");
    
    
    sancus_enable(&led_app);
    dump_sm_layout(&led_app);
    sancus_enable(&led_driver);
    dump_sm_layout(&led_driver);
    sancus_enable(&timerA_driver);
    dump_sm_layout(&timerA_driver);
    sancus_enable(&IVT);
    dump_sm_layout(&IVT);

    led_init();
   

    printf("interrupts enabled\n");
    asm("eint");

    while(1)
    {
    }
    
}

