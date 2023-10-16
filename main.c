#include "MKL46Z4.h"
#include <stdbool.h>

// LED (RG)
// LED_GREEN = PTD5
// LED_RED = PTE29

uint8_t state  = 0; //0 = 00, 1 = 01, 2 = 10, 3 = 11
bool l1 = false, l2 = false;  

void state_machine(bool a);

void PORTDIntHandler(void){

  bool a = PORTC->PCR[3]>>24, b = PORTC->PCR[12]>>24;

  if (a) {
    state_machine(0);    
  } else {
    if(b)
      state_machine(1);
  }

  PORTC->PCR[12] |= PORT_PCR_ISF(1);
  PORTC->PCR[3] |= PORT_PCR_ISF(1);

}

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
void led_red_init(void)
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
  PORTC->PCR[3] |= PORT_PCR_IRQC(10);
  GPIOC->PDDR &= ~(1 << 3);

  NVIC_EnableIRQ(PORTC_PORTD_IRQn);

}

void b_sw2_init(){
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[12] |= PORT_PCR_MUX(1);
  PORTC->PCR[12] |= PORT_PCR_PE(1);
  PORTC->PCR[12] |= PORT_PCR_PS(1);
  PORTC->PCR[12] |= PORT_PCR_ISF(1);
  PORTC->PCR[12] |= PORT_PCR_IRQC(0b1010);
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

  void lights(bool a){
    switch (a){
      case 0:
        if(l1)led_green_toggle();
        if(!l2)led_red_toggle();
        l1 = false;
        l2 = true;   
        break;
      case 1:
        if(!l1)led_green_toggle();
        if(l2)led_red_toggle();
        l1 = true;
        l2 = false;  
        break;
    }
  }


void state_machine(bool a){
  //////////////////
  a =a;
    switch (state){
      case 0: //0 = 00, 1 = 01, 2 = 10, 3 = 11     

        if ( !a ){
          state = 1;
        }
        if ( a ){
          state = 2;
        }
        lights(0);
        break;
      case 1:
        if ( !a ){
          state = 0;
          lights(0);
        }
        if ( a ){
          state = 3;
          lights(1);
        }
        break;
      case 2: 
        if(l1)led_green_toggle();
        if(!l2)led_red_toggle();
        l1 = false;
        l2 = true;        

        if ( !a ){
          state = 3;
          lights(1);
        }
        if ( a ){
          state = 0;
          lights(0);
        }
        break;
      case 3: 
        if(!l1)led_green_toggle();
        if(l2)led_red_toggle();
        l1 = true;
        l2 = false;        

        if ( !a ){
          state = 2;
        }
        if ( a ){
          state = 1;
        }
        lights(0);  
        break;
    }

  }


int main(void)
{
  led_green_init();
  led_red_init();
  b_sw1_init();
  b_sw2_init();

  lights(0);

  while(1){
    
  }

  return 0;
}
 

