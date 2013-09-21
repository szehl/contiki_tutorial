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

/*
 * We must have somewhere to put incoming data, and we use a 10 byte
 * buffer for this purpose.
 */
static char buffer[10];

/*---------------------------------------------------------------------------*/
/*
 * A protosocket always requires a protothread. The protothread
 * contains the code that uses the protosocket. We define the
 * protothread here.
 */
 
static
PT_THREAD(handle_connection(struct psock *p))
{
  /*
   * A protosocket's protothread must start with a PSOCK_BEGIN(), with
   * the protosocket as argument.
   *
   * Remember that the same rules as for protothreads apply: do NOT
   * use local variables unless you are very sure what you are doing!
   * Local (stack) variables are not preserved when the protothread
   * blocks.
   */
  PSOCK_BEGIN(p);

  /*
   * We start by sending out a welcoming message. The message is sent
   * using the PSOCK_SEND_STR() function that sends a null-terminated
   * string.
   */
  PSOCK_SEND_STR(p, "Welcome, please type something and press return.\n");
  
  /*
   * Next, we use the PSOCK_READTO() function to read incoming data
   * from the TCP connection until we get a newline character. The
   * number of bytes that we actually keep is dependant of the length
   * of the input buffer that we use. Since we only have a 10 byte
   * buffer here (the buffer[] array), we can only remember the first
   * 10 bytes received. The rest of the line up to the newline simply
   * is discarded.
   */
  PSOCK_READTO(p, '\n');
  
  /*
   * And we send back the contents of the buffer. The PSOCK_DATALEN()
   * function provides us with the length of the data that we've
   * received. Note that this length will not be longer than the input
   * buffer we're using.
   */
  PSOCK_SEND_STR(p, "Got the following data: ");
  PSOCK_SEND(p, buffer, PSOCK_DATALEN(p));
  PSOCK_SEND_STR(p, "Good bye!\r\n");

  /*
   * We close the protosocket.
   */
  PSOCK_CLOSE(p);

  /*
   * And end the protosocket's protothread.
   */
  PSOCK_END(p);
}



PROCESS_THREAD(example_psock_server_process, ev, data)
{
  PROCESS_BEGIN();
  /*Festlegung des Listening Ports*/
  tcp_listen(UIP_HTONS(1010));

  while(1) {
	/*Warte bis ein Packet eingetroffen ist*/
    PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
	/*Wenn eine Verbindung besteht, initialisiere den Protosocket*/
    if(uip_connected()) {
      /*Protosocket Initialisierung*/
      PSOCK_INIT(&ps, buffer, sizeof(buffer));

       /*loop until the connection is aborted, closed, or times out*/
      while(!(uip_aborted() || uip_closed() || uip_timedout())) {

		/*Warte bis das nächste Packet eingetroffen ist*/
        PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
		/*Aufruf der Handler-Funktion*/
        handle_connection(&ps);
      }
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
