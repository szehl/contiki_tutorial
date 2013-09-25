#include "contiki.h"
#include <stdio.h>
#include <avr/io.h>
	
PROCESS(switch_led_on, "LED einschalten nach Booten");
AUTOSTART_PROCESSES(&switch_led_on);
	
PROCESS_THREAD(switch_led_on, ev, data)
{
	PROCESS_BEGIN();
	/*Pin als Ausgang schalten*/
	DDRB |= (1 << PIN7);
	/*Pin auf Low Pegel schalten*/
	PORTB &= ~(1 << PIN7);
	
	PROCESS_END();
}
