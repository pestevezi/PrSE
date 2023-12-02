#include <stdbool.h>
#include "MKL46Z4.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lcd.h"
#include "queue.h"
#include <string.h>

int count; int length; char sent; char rec; char cadena[20] = "Esta es la cadena";
QueueHandle_t xQueue1; int temp;

void irclk_ini()
{
  MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
  MCG->C2 = MCG_C2_IRCS(0); //0 32KHZ internal reference clock; 1= 4MHz irc

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

void led_green_init()
{
	SIM_COPC = 0;
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
	PORTD_PCR5 = PORT_PCR_MUX(1);
	GPIOD_PDDR |= (1 << 5);
	GPIOD_PSOR = (1 << 5);
}

void led_green_toggle()
{
	GPIOD_PTOR = (1 << 5);
}

void led_red_init()
{
	SIM_COPC = 0;
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
	PORTE_PCR29 = PORT_PCR_MUX(1);
	GPIOE_PDDR |= (1 << 29);
	GPIOE_PSOR = (1 << 29);
}

void led_red_toggle(void)
{
	GPIOE_PTOR = (1 << 29);
}

void PORTDIntHandler(void){

  bool a = PORTC->PCR[3]>>24, b = PORTC->PCR[12]>>24;

  if (a){

  } else {

  }
  lcd_display_dec(999);

  PORTC->PCR[12] |= PORT_PCR_ISF(1);
  PORTC->PCR[3] |= PORT_PCR_ISF(1);

}

void productor(void *pvParameters)
{
    for (;;) {
        led_green_toggle();

        if (count < length){
	        sent = cadena[count]; count++;
	        const void * ttt = (void *)&sent; 
	//    	lcd_display_dec(count);
			
	//		QueueHandle_t q = *((QueueHandle_t *)pvParameters);


	    	if( xQueue1 != 0 ){
		    	xQueueSendToBack(xQueue1, ttt, portMAX_DELAY);
	    	}

    	}


		temp = uxQueueMessagesWaiting(xQueue1);
		lcd_display_dec(temp);

        vTaskDelay(200/portTICK_RATE_MS);
    }
}

void consumidor(void *pvParameters)
{
    vTaskDelay(100/portTICK_RATE_MS);
    for (;;) {
        led_red_toggle();
        xQueueReceive(xQueue1, &rec, portMAX_DELAY);
//    	lcd_display_dec(rec);
    }
}
/*
void counter(void *pvParameters)
{
	for(;;){
	    vTaskDelay(400/portTICK_RATE_MS);		
    	lcd_display_dec(temp);
	}
}
*/
int main(void)
{

	count = 0; temp = 0;
	length = strlen(cadena);

	irclk_ini(); // Enable internal ref clk to use by LCD
  	lcd_ini();

	led_green_init();
	led_red_init();

//	b_sw1_init();
//	b_sw2_init();

//	lcd_display_dec(count);
	xQueue1 = xQueueCreate(20, sizeof(char));


	/* create green led task */
	xTaskCreate(productor, (signed char *)"productor", 
		configMINIMAL_STACK_SIZE, (void *)NULL, 2, NULL);

	/* create red led task */
	xTaskCreate(consumidor, (signed char *)"consumidor", 
		configMINIMAL_STACK_SIZE, (void *)NULL, 1, NULL);
	
//	xTaskCreate(counter, (signed char *)"counter", 
//		configMINIMAL_STACK_SIZE, (void *)NULL, 3, NULL);

	/* start the scheduler */
	vTaskStartScheduler();


	/* should never reach here! */
	for (;;);

	return 0;
}

