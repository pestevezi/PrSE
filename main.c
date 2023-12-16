#include "MKL46Z4.h"
#include "lcd.h"
#include <stdbool.h>

// LED (RG)
// LED_GREEN = PTD5 (pin 98)
// LED_RED = PTE29 (pin 26)

// SWICHES
// RIGHT (SW1) = PTC3 (pin 73)
// LEFT (SW2) = PTC12 (pin 88)

// Enable IRCLK (Internal Reference Clock)
// see Chapter 24 in MCU doc

int minute, second, state, timeon = 400;
const int total = 400;
bool ini = true, on = true;

void delay(void)
{
  volatile int i;

  for (i = 0; i < 1000000; i++);
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
int sw1_check()
{
  return( !(GPIOC->PDIR & (1 << 3)) );
}

int sw2_check()
{
  return( !(GPIOC->PDIR & (1 << 12)) );
}

// RIGHT_SWITCH (SW1) = PTC3
// LEFT_SWITCH (SW2) = PTC12
void sws_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[3] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1);
  PORTC->PCR[12] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1);
  GPIOC->PDDR &= ~(1 << 3 | 1 << 12);
}

// LED_GREEN = PTD5
void led_green_ini()
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
void led_red_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
  PORTE->PCR[29] = PORT_PCR_MUX(1);
  GPIOE->PDDR |= (1 << 29);
  GPIOE->PSOR = (1 << 29);
}

void led_red_toggle(void)
{
  GPIOE->PTOR = (1 << 29);
}

// LED_RED = PTE29
// LED_GREEN = PTD5
void leds_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;
  PORTD->PCR[5] = PORT_PCR_MUX(1);
  PORTE->PCR[29] = PORT_PCR_MUX(1);
  GPIOD->PDDR |= (1 << 5);
  GPIOE->PDDR |= (1 << 29);
  // both LEDS off after init
  GPIOD->PSOR = (1 << 5);
  GPIOE->PSOR = (1 << 29);
}

void irclk_ini()
{
  MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
  MCG->C2 = MCG_C2_IRCS(0); //0 32KHZ internal reference clock; 1= 4MHz irc
}

void init_tpm(){

  SIM->SCGC6 |= SIM_SCGC6_TPM0(1); 
  SIM->SOPT2 |= SIM_SOPT2_TPMSRC(3);

//  TPM0->MOD=32000;
  TPM0->MOD=total;
  TPM0->CNT=0;
  TPM0->SC &= TPM_SC_PS(0);
  TPM0->SC |= TPM_SC_TOIE(1);
  TPM0->SC |= TPM_SC_CMOD(1);
/*
  TPM0->SC |= TPM_SC_CPWMS(1);
  TPM0->CONTROLS[0].CnV |= TPM_CnV_VAL(10);
  TPM0->CONTROLS[0].CnSC |= TPM_CnSC_CHIE(1);
*/

  NVIC_EnableIRQ(TPM0_IRQn);

}

void FTM0IntHandler(void){
  led_red_toggle();

  if (on){
    TPM0->MOD=timeon;
    on = false;
  }
  else{
    TPM0->MOD=total-timeon;
    on = true;
  }
  TPM0->SC |= TPM_SC_TOF(1);

}

void PORTDIntHandler(void) {

  bool a = PORTC->PCR[3]>>24, b = PORTC->PCR[12]>>24;


//  TPM0->SC |= TPM_SC_TOF(0);

  if (a) {
    if(timeon > 0)
      timeon -= 50;
  }
  if(b){
    if (timeon < 400)
      timeon += 50;
  }

//  TPM0->SC |= TPM_SC_TOF(1);

  PORTC->PCR[12] |= PORT_PCR_ISF(1);
  PORTC->PCR[3] |= PORT_PCR_ISF(1);

}

int main(void)
{
  leds_ini();
  irclk_ini(); // Enable internal ref clk to use by LCD
  lcd_ini();
  b_sw1_init();
  b_sw2_init();
  init_tpm();

  //minute = 0; second = 0;
  //lcd_display_time(minute, second);

  while (1) {
//    if (!ini)
//      lcd_display_time(minute, second);
    lcd_display_dec(total - timeon);
  }

  return 0;
}
