#include <sancus/sm_support.h>
#include <msp430f1121.h>
#include <stdio.h>
#include <sancus_support/uart.h>
#include <sancus/reactive_stubs_support.h>
#include "sm_io_wrap.h"

// Implementation of SECURE scrolling text through Timer_A interrupts

#define _bis_SR_register(x) \
    __asm__ __volatile__ ("bis.w %0, r2" \
        : : "ri"((unsigned int) x) \
    )

DECLARE_SM(led_app,0x1234);
DECLARE_SM(led_driver,0x1234);
DECLARE_SM(timerA_driver,0x1234);
SM_HANDLE_IRQ(led_app,9);

// TODO: led_app should check if it was assigned the ID the naked SMs expect
void SM_NAKED_ENTRY(led_driver) write_LED(uint8_t data, uint8_t led)
{
    asm ("add.b (%0), r14\n\t"
         "mov.b r15, -1(r14)\n\t"
         : 
         :"m" (led_driver.secret_start));
}

uint8_t SM_NAKED_ENTRY(led_driver) read_LED(uint8_t led)
{
    asm("add.b (%0),r15\n\t"
        "mov.b -1(r15),r15\n\t"        
        :
        :"m" (led_driver.secret_start));
}

void SM_NAKED_ENTRY(timerA_driver) write_timerA(uint16_t data, uint16_t address)
{
    asm("mov.w r15, @r14");
}

// TODO: can an attacker still mask timerA interrupts?
// TODO: non-maskable interrupts
void SM_FUNC(led_app) timerA_init(void)
{
    write_timerA(CCIE,TACCTL0_);
    write_timerA(TASSEL_2 + MC_2 + ID_3, TACTL_);
    write_timerA(62500,TACCR0_);
}

void SM_ENTRY(led_app) led_init(void)
{
    printf0("initiating leds\n");
    write_LED(0x71,0x1); // 'F'
    write_LED(0x79,0x2); // 'E'
    write_LED(0x79,0x3); // 'E'
    write_LED(0x7c,0x4); // 'b'
    write_LED(0x5e,0x5); // 'd'
    write_LED(0x77,0x6); // 'A'
    write_LED(0x79,0x7); // 'E'
    write_LED(0x5E,0x8); // 'd'
    timerA_init(); 
}

void SM_FUNC(led_app) cycle_leds(void)
{
    //printf0("cycling leds\n");
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
// TODO: check what happens if attacker code jumps to this ISR
// TODO: check what happens if we assign another (non-SM) ISR to respond to timerA interrupts
// TODO: check the stubs for SM ISR's, what happens exactly (is this entry point now defined at the IVT?)
SM_ISR(led_app,num)
{ 
    timerCount = (timerCount +1)%40;
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
    
    // TODO: order important here?
    sancus_enable(&led_app);
    dump_sm_layout(&led_app);
    sancus_enable(&led_driver);
    dump_sm_layout(&led_driver);
    sancus_enable(&timerA_driver);
    dump_sm_layout(&timerA_driver);

    led_init();
   
    // This macro inside an SM causes processor crash?
    //_bis_SR_register(GIE); // LPM0 w/ interrupt
    asm("eint");

    printf("maskable interrupts enabled\n");
    //write_timerA(0x0000,TACCTL0_); // attacker
    while(1)
    {
    led_init(); // Attacker continuously calls entry point
    }
    
}

