#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include <stdio.h> 
#include <avr/io.h>
#include <string.h>



PROCESS(example_psock_server_process, "Example protosocket server");

PROCESS(switch_led_on, "Turns on the LED when device is powered on");


AUTOSTART_PROCESSES(&switch_led_on, &example_psock_server_process);

static struct psock ps;

static char buffer[36];


static PT_THREAD(handle_connection(struct psock *p))
{
  PSOCK_BEGIN(p);
  PSOCK_SEND_STR(p, "Welcome, your request will be processed...\n");
  PSOCK_READTO(p, '\n');
  printf("TCP Server received: %s \n",buffer);
  PSOCK_SEND_STR(p, "You wrote: ");
  PSOCK_SEND(p, buffer, PSOCK_DATALEN(p));
  PSOCK_SEND_STR(p, "Good bye!\r\n");
  PSOCK_CLOSE(p);
  PSOCK_END(p);
}
/*---------------------------------------------------------------------------*/


PROCESS_THREAD(example_psock_server_process, ev, data)
{
 PROCESS_BEGIN();
 /*Festlegung des Listening Ports*/
 tcp_listen(UIP_HTONS(1010));
 while(1) 
 {
  printf("TCP Server is running\n");
  /*Warte bis ein Packet eingetroffen ist*/
  PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
   /*Bei Verbindungsanfrage initialisiere Protosocket*/
   if(uip_connected()) 
   {
    PSOCK_INIT(&ps, buffer, sizeof(buffer));
    while(!(uip_aborted()||uip_closed()||uip_timedout()))
    {
     PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
      handle_connection(&ps);
    }/*Aufruf der Handler-Funktion*/
   }
 }
 PROCESS_END();
}


PROCESS_THREAD(switch_led_on, ev, data)
{
	PROCESS_BEGIN();
	
	/*Switch LED on @ANY Brick On if SNMPD started and set other LEDs as Output*/
	DDRB |= (1 << PIN5);
	DDRB |= (1 << PIN6);
	DDRB |= (1 << PIN7);
	PORTB &= ~(1 << PIN7);
	PORTB |= (1 << PIN6);
	PORTB |= (1 << PIN5);
	/*END LED @ANY Brick*/
	printf("LED is on!\n");
	
	PROCESS_END();
}
