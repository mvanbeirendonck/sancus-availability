#include <sancus/sm_support.h>
#include <msp430f1121.h>
#include <stdio.h>
#include <sancus_support/uart.h>
#include <sancus/reactive_stubs_support.h>
#include "sm_io_wrap.h"
#include <sancus_support/tsc.h>

// MMIO registers for led digit segments:
// (MSB-1) g f e d c b a (LSB)
#define LED1 0x0090 //LED1 is the value at 0x0090
#define LED2 0x0091
#define LED3 0x0092
#define LED4 0x0093
#define LED5 0x0094
#define LED6 0x0095
#define LED7 0x0096
#define LED8 0x0097

DECLARE_SM(led_app,0x1234);
DECLARE_EXCLUSIVE_MMIO_SM(led_driver,0x0090,0x0098,2,0x1234); // End address is exclusive
DECLARE_EXCLUSIVE_MMIO_SM(timerA_driver,0x012E,0x0177,2,0x1234);
DECLARE_MMIO_SM(IVT,0xFFE0,0xFFFF,0x1234);
SM_HANDLE_IRQ(led_app,9);


DECLARE_TSC_TIMER(overhead_timer);


void SM_MMIO_ENTRY(led_driver) write_LED(uint8_t data, uint8_t led)
{
    asm("mov.b r15, @r14");
}

uint8_t SM_MMIO_ENTRY(led_driver) read_LED(uint8_t led)
{
    asm("mov.b @r15, r15");
}

void SM_MMIO_ENTRY(timerA_driver) write_timerA(uint16_t data, uint16_t address)
{
    asm("mov.w r15, @r14");
}

void SM_FUNC(led_app) timerA_init(void)
{
    write_timerA(CCIE,TACCTL0_);
    write_timerA(TASSEL_2 + MC_1, TACTL_);
    write_timerA(1,TACCR0_);
}

void SM_ENTRY(led_app) led_init(void)
{
    
    write_LED(0x71,LED1); // 'F'
    write_LED(0x79,LED2); // 'E'
    write_LED(0x79,LED3); // 'E'
    write_LED(0x7c,LED4); // 'b'
    write_LED(0x5e,LED5); // 'd'
    write_LED(0x77,LED6); // 'A'
    write_LED(0x79,LED7); // 'E'
    write_LED(0x5E,LED8); // 'd'
    timerA_init(); 
}

void SM_FUNC(led_app) cycle_leds(void)
{
    uint8_t foo = read_LED(LED1);
    write_LED(read_LED(LED2),LED1);
    write_LED(read_LED(LED3),LED2);
    write_LED(read_LED(LED4),LED3);
    write_LED(read_LED(LED5),LED4);
    write_LED(read_LED(LED6),LED5);
    write_LED(read_LED(LED7),LED6);
    write_LED(read_LED(LED8),LED7);
    write_LED(foo,LED8);
}



SM_ISR(led_app,num)
{ 
    cycle_leds();
}

int main()
{
    WDTCTL = WDTPW + WDTHOLD;
    uart_init();

    printf("main started\n");
    
    sancus_enable(&IVT);
    sancus_enable(&led_app);
    sancus_enable(&led_driver);
    sancus_enable(&timerA_driver);

    led_init();

    while(1) //in while because first call might be more overhead, before caller ID is stored
	{
	TSC_TIMER_START(overhead_timer);
    	asm("eint");
    	asm("dint");
    	TSC_TIMER_END(overhead_timer); //result = interval - 2 = 2963 -6 = 2957
	}
}

