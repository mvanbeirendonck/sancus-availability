#include <sancus/sm_support.h>
#include <msp430f1121.h>
#include <stdio.h>
#include <sancus_support/uart.h>
#include <sancus/reactive_stubs_support.h>
#include "sm_io_wrap.h"

#define LED1 *(volatile uint8_t*) 0x0090 
#define LED2 *(volatile uint8_t*) 0x0091
#define LED3 *(volatile uint8_t*) 0x0092
#define LED4 *(volatile uint8_t*) 0x0093
#define LED5 *(volatile uint8_t*) 0x0094
#define LED6 *(volatile uint8_t*) 0x0095
#define LED7 *(volatile uint8_t*) 0x0096
#define LED8 *(volatile uint8_t*) 0x0097

#define IVT_start (uint8_t*) 0xffe0
#define IVT_end (uint8_t*) 0xfffe

DECLARE_SM(led_app,0x1234);
SM_HANDLE_IRQ(led_app,9);

DECLARE_EXCLUSIVE_MMIO_SM(led_driver,0x0090,0x0098,1,0x1234);
DECLARE_EXCLUSIVE_MMIO_SM(timerA_driver,0x012E,0x0178,1,0x1234);

DECLARE_SM(IVT,0x1234);

void SM_ENTRY(IVT) IVT_entry(void) {
}

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
    // Initialize TimerA
    write_timerA(62500,TACCR0_);
    write_timerA(CCIE,TACCTL0_);
    write_timerA(TASSEL_2 + MC_2 + ID_3, TACTL_);
    asm("eint");
}

SM_DATA(led_app) int ledInit=0;

void SM_ENTRY(led_app) led_init(void)
{
    if (ledInit==0) // Protect from unauthorized calling
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

        // Initialize Port 1 as input button

	P1DIR = 0;
	P1SEL = 0;
	P1IE = 0;
	P1IES = 0;
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
    timerCount = (timerCount+1)%40; // Cycles after 1 second
    if (timerCount == 0)
    {
        cycle_leds();
    }

}


/*
Alternative to polling for button press.
*/
void __attribute__((__interrupt__ (0x0004))) Attacker(void)
{
      asm("dint");
      LED1 = 0x77; // 'A'
      LED2 = 0x76; // 'H'
      LED3 = 0x77; // 'A'
      LED4 = 0x76; // 'H'
      LED5 = 0x77; // 'A'
      LED6 = 0x76; // 'H'
      LED7 = 0x77; // 'A'
      LED8 = 0x76; // 'H'
      while(1);
}

int main()
{
    WDTCTL = WDTPW + WDTHOLD;
    uart_init();

    
       
    sancus_enable(&led_app);
    sancus_enable(&led_driver);
    sancus_enable(&timerA_driver);

    IVT.public_start = IVT_start;
    IVT.public_end = IVT_end;
    sancus_enable(&IVT);
    dump_sm_layout(&IVT);

    led_init();

    // Attacker tries to overwrite LEDs on button press and disable LED cycling.
    while(1)
    {
      if ((P1IN & 1) == 1) //BTN1
      {
      LED1 = 0x77; // 'A'
      LED2 = 0x76; // 'H'
      LED3 = 0x77; // 'A'
      LED4 = 0x76; // 'H'
      LED5 = 0x77; // 'A'
      LED6 = 0x76; // 'H'
      LED7 = 0x77; // 'A'
      LED8 = 0x76; // 'H'
      asm("dint");
      }
    }
    
}

