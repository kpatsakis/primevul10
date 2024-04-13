PROCESS_THREAD(snmp_process, ev, data)
{
  PROCESS_BEGIN();

  snmp_packet_t snmp_packet;

  /* new connection with remote host */
  snmp_udp_conn = udp_new(NULL, 0, NULL);
  udp_bind(snmp_udp_conn, SNMP_SERVER_PORT);
  LOG_DBG("Listening on port %u\n", uip_ntohs(snmp_udp_conn->lport));

  while(1) {
    PROCESS_YIELD();

    if(ev != tcpip_event) {
      continue;
    }

    if(!uip_newdata()) {
      continue;
    }

    LOG_DBG("receiving UDP datagram from [");
    LOG_DBG_6ADDR(&UIP_IP_BUF->srcipaddr);
    LOG_DBG_("]:%u", uip_ntohs(UIP_UDP_BUF->srcport));
    LOG_DBG_(" Length: %u\n", uip_datalen());

    /* Setup SNMP packet */
    snmp_packet.in = (uint8_t *)uip_appdata;
    snmp_packet.used = uip_datalen();

    snmp_packet.out = (uint8_t *)(uip_appdata + UIP_BUFSIZE - UIP_IPUDPH_LEN);
    snmp_packet.max = UIP_BUFSIZE - UIP_IPUDPH_LEN;

    /* Handle the request */
    if(!snmp_engine(&snmp_packet)) {
      LOG_DBG("Error while handling the request\n");
      continue;
    }

    LOG_DBG("Sending response\n");
    /* Send the response */
    uip_udp_packet_sendto(snmp_udp_conn, snmp_packet.out, snmp_packet.used, &UIP_IP_BUF->srcipaddr, UIP_UDP_BUF->srcport);
  } /* while (1) */

  PROCESS_END();
}