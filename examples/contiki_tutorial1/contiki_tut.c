#include "contiki.h"
#include <stdio.h> 
#include <avr/io.h>

PROCESS(pt_example, "Protothread Example Process");
PROCESS(switch_led_on, "Turns on the LED when device is powered on");

AUTOSTART_PROCESSES(&pt_example, &switch_led_on);


PROCESS_THREAD(switch_led_on, ev, data)
{
	PROCESS_BEGIN();
	

	DDRB |= (1 << PIN7);
	PORTB &= ~(1 << PIN7);
	printf("LED is on!\n");
	
	PROCESS_END();
}

PROCESS_THREAD(pt_example, ev, data)
{
	
   /*Timer Variable statisch deklariert, damit sie nach dem Kontextwechsel noch verfügbar ist*/
   static struct etimer et;
   
   PROCESS_BEGIN();

   etimer_set(&et, (CLOCK_SECOND*20));
   
   while(1) {
     /*Warte nun bis die 20s abgelaufen sind, solange können andere Protothreads den Prozessor nutzen*/
     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
     
     etimer_reset(&et);
     
     printf("Timer abgelaufen, starte Timer neu...\n");
   }
   PROCESS_END();
}
