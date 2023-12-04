#include "MKL46Z4.h"
#include "lcd.h"

// LED (RG)
// LED_GREEN = PTD5 (pin 98)
// LED_RED = PTE29 (pin 26)

// SWICHES
// RIGHT (SW1) = PTC3 (pin 73)
// LEFT (SW2) = PTC12 (pin 88)

// Enable IRCLK (Internal Reference Clock)
// see Chapter 24 in MCU doc

int minute, second;

void delay(void)
{
  volatile int i;

  for (i = 0; i < 1000000; i++);
}

void countdown(){
    if (second > 0){
    second--;
  } else {
    if (minute > 0){
      minute--;
      second = 59;
    }
    else
      ;
  }
}

// RIGHT_SWITCH (SW1) = PTC3
void sw1_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[3] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1);
  GPIOC->PDDR &= ~(1 << 3);
}

// LEFT_SWITCH (SW2) = PTC12
void sw2_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[12] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1);
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

  MCG->C2 &= MCG_C2_IRCS(0);

  SIM->SCGC6 |= SIM_SCGC6_TPM0(1); 
  SIM->SOPT2 |= SIM_SOPT2_TPMSRC(3);

  TPM0->MOD=0x9FFFF0;// TPM0->CNT=0;  
  TPM0->SC &= TPM_SC_PS(0);
  TPM0->SC |= TPM_SC_TOIE(1);
  TPM0->SC |= TPM_SC_CMOD(1);
  //TPM0->SC &= TPM_SC_CPWMS(0);
  //TPM0->CONTROLS[0].CnSC  |= TPM_CnSC_CHIE(1);
  //TPM0->SC |= TPM_SC_TOF(1);

  //SIM->SOPT2 = 0x03000000;

  //MCG->C1 |= 0x2;

  NVIC_EnableIRQ(TPM0_IRQn);
}

void FTM0IntHandler(void){
  led_red_toggle();
  countdown();
  TPM0->SC |= TPM_SC_TOF(1);
  TPM0->CNT=0;
}

void init_pit(){
  SIM->SCGC6 |= SIM_SCGC6_PIT(1);
  PIT->MCR &= PIT_MCR_MDIS(0);

  PIT->CHANNEL[0].LDVAL = 0x9FFFF0;//PIT_LDVAL_TSV(10900000);
  PIT->CHANNEL[0].TCTRL &= PIT_TCTRL_CHN(0);
  PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN(1);
  PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE(1);
//  PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF(1);
  NVIC_SetPriority(PIT_IRQn, 3); 
  NVIC_ClearPendingIRQ(PIT_IRQn);
  NVIC_EnableIRQ(PIT_IRQn);
}

void PITIntHandler(void) {
  led_green_toggle(); 

  countdown();

  PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF(1); //clear the flag
}


int main(void)
{
  leds_ini();
  irclk_ini(); // Enable internal ref clk to use by LCD
  lcd_ini();

    //
  init_tpm();
  init_pit();
    //
  
  minute = 1; second = 5;

  while (1) {
    lcd_display_time(minute, second);
    //lcd_display_dec(TPM0->STATUS);
  }

  return 0;
}
