#include "contiki.h"
#include "contiki-net.h"
#include <stdio.h> 
#include <avr/io.h>


#define UDP_IP_BUF   ((struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN])
#define MAX_PAYLOAD_LEN 120

PROCESS(pt_example, "Protothread Example Process");
PROCESS(switch_led_on, "Turns on the LED when device is powered on");
PROCESS(example_udp_server_process, "Example UDP Server Prozess");

AUTOSTART_PROCESSES(&pt_example, &switch_led_on, &example_udp_server_process);


PROCESS_THREAD(switch_led_on, ev, data)
{
	PROCESS_BEGIN();
	DDRB |= (1 << PIN7);
	PORTB &= ~(1 << PIN7);
	/*END LED @ANY Brick*/
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

static struct uip_udp_conn *udpconn;

PROCESS_THREAD(example_udp_server_process, ev, data)
{ 
 
  PROCESS_BEGIN();
  /*Starte neue UDP Verbindung mit IP 0.0.0.0 und Port 0, */
  /* d.h. akzeptiere jede ankommende Verbindung*/
  udpconn = udp_new(NULL, UIP_HTONS(0), NULL); 
  /*Setze den Port auf dem gelauscht wird auf 50000*/
  /*HTONS() übersetzt zu Network Byte Order*/
  udp_bind(udpconn, UIP_HTONS(50000)); 
  printf("listening on UDP port %u\n", UIP_HTONS(udpconn->lport));
  while(1) {
    /* Warte bis ein TCP/IP event eintrifft */
    PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
    /*Rufe die Handler-Funktion auf*/
    bsp_udphandler();
  }
  PROCESS_END();
}


void bsp_udphandler(void)
{
  char buf[MAX_PAYLOAD_LEN];
  if(uip_newdata()) 
  {
    /*Zeige Benutzer den Inhalt der empfangenen Nachricht*/
    /*Setze letztes Byte zu Null, für String Ende*/
    ((char *)uip_appdata)[uip_datalen()] = 0;
    printf("Server received: '%s'", (char *)uip_appdata);
    /*Verwende die Quell- als Zieladresse für Antwort */
    uip_ipaddr_copy(&udpconn->ripaddr, &UDP_IP_BUF->srcipaddr);
    udpconn->rport = UDP_IP_BUF->srcport;
    /*Schreibe Antwort Daten in Buffer*/
    sprintf(buf, "Nothing special, just sending some messages...");
    /*Versende das Antwort Packet*/
    uip_udp_packet_send(udpconn, buf, strlen(buf));
    /*Setze Adresse/Port in Verbindungsstruktur auf Null,*/
    /*um von jedem Absender Daten empfangen zu können*/
    memset(&udpconn->ripaddr, 0, sizeof(udpconn->ripaddr));
    udpconn->rport = 0;
  }
}
