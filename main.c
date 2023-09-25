#include "MKL46Z4.h"
#include <stdbool.h>
uint8_t state;//0 = 00, 1 = 01, 2 = 10, 3 = 11
// LED (RG)
// LED_GREEN = PTD5
// LED_RED = PTE29

void delay(void)
{
  volatile int i;

  for (i = 0; i < 1000000; i++);
}

// LED_GREEN = PTD5
void led_green_init()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
  PORTD->PCR[5] = PORT_PCR_MUX(1);
  GPIOD->PDDR |= (1 << 5);
  GPIOD->PSOR = (1 << 5);
}

void led_green_toggle()
{
  GPIOD->PTOR = (1 << 5);
}

// LED_RED = PTE29
void led_red_init()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
  PORTE->PCR[29] = PORT_PCR_MUX(1);
  GPIOE->PDDR |= (1 << 29);
  GPIOE->PSOR = (1 << 29);
}

void b_sw1_init(){
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	PORTC->PCR[3] |= PORT_PCR_MUX(1);
  PORTC->PCR[3] |= PORT_PCR_PE(1);
  PORTC->PCR[3] |= PORT_PCR_PS(1);
  GPIOC->PDDR &= ~(1 << 3);
}

void b_sw2_init(){
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[12] |= PORT_PCR_MUX(1);
  PORTC->PCR[12] |= PORT_PCR_PE(1);
  PORTC->PCR[12] |= PORT_PCR_PS(1);
  GPIOC->PDDR &= ~(1 << 12);
}

void led_red_toggle(void)
{
  GPIOE->PTOR = (1 << 29);
}

bool sw1_check(){
  return ((( (GPIOC->PDIR) >> 3) % 2) == 0 );
}

bool sw2_check(){
    return ((( (GPIOC->PDIR) >> 12) % 2) == 0 );
}

int main(void)
{
  led_green_init();
  led_red_init();
  b_sw1_init();
  b_sw2_init();
  state = 0;
  bool l1 = false, l2 = false;

  while (1) {
    switch (state){
      case 0: //0 = 00, 1 = 01, 2 = 10, 3 = 11

        if(l1)led_green_toggle();
        if(!l2)led_red_toggle();
        l1 = false;
        l2 = true;        

        if ( sw1_check() ){
          state = 1;
          while(sw1_check());          
        }
        if ( sw2_check() ){
          state = 2;
          while(sw2_check());          
        }
        break;
      case 1:
        if(l1)led_green_toggle();
        if(!l2)led_red_toggle();
        l1 = false;
        l2 = true;        

        if ( sw1_check() ){
          state = 0;
          while(sw1_check());          
        }
        if ( sw2_check() ){
          state = 3;
          while(sw2_check());
        }
        break;
      case 2: 
        if(l1)led_green_toggle();
        if(!l2)led_red_toggle();
        l1 = false;
        l2 = true;        

        if ( sw1_check() ){
          state = 3;
          while( sw1_check() );
        }
        if ( sw2_check() ){
          state = 0;
          while( sw2_check() );          
        }
        break;
      case 3: 
        if(!l1)led_green_toggle();
        if(l2)led_red_toggle();
        l1 = true;
        l2 = false;        

        if ( sw1_check() ){
          state = 2;
          while(sw1_check());          
        }
        if ( sw2_check() ){
          state = 1;
          while(sw2_check());          
        }
        break;
    }
  }

  return 0;
}
 

