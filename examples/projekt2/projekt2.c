#include "contiki.h"
#include <stdio.h>
#include <avr/io.h>
#include "contiki-net.h"

#define UDP_IP_BUF ((struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN])
#define MAX_PAYLOAD_LEN 120
static struct uip_udp_conn *udpconn;

PROCESS(udp_server_process, "UDP Server");
AUTOSTART_PROCESSES(&udp_server_process);

PROCESS_THREAD(udp_server_process, ev, data)
{ 
  PROCESS_BEGIN();
  DDRB |= (1 << PIN6);
  PORTB |= (1 << PIN6);
  DDRB |= (1 << PIN7);
  PORTB &= ~(1 << PIN7);
  udpconn = udp_new(NULL, UIP_HTONS(0), NULL); 
  udp_bind(udpconn, UIP_HTONS(50000)); 
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
    udphandler();
  }
  PROCESS_END();
}



void udphandler(void)
{
 char buf[MAX_PAYLOAD_LEN];
 if(uip_newdata()) 
  {
  PORTB &= ~(1 << PIN6);
  uip_ipaddr_copy(&udpconn->ripaddr, &UDP_IP_BUF->srcipaddr);
  udpconn->rport = UDP_IP_BUF->srcport;
  sprintf(buf, "Ich bin eine Antwort");
  uip_udp_packet_send(udpconn, buf, strlen(buf));
  memset(&udpconn->ripaddr, 0, sizeof(udpconn->ripaddr));
  udpconn->rport = 0;
  PORTB |= (1 << PIN6);
 }
}
