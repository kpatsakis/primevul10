static int ndpi_search_tls_udp(struct ndpi_detection_module_struct *ndpi_struct,
			       struct ndpi_flow_struct *flow) {
  struct ndpi_packet_struct *packet = &flow->packet;
  // u_int8_t handshake_type;
  u_int32_t handshake_len;
  u_int16_t p_len;
  const u_int8_t *p;

#ifdef DEBUG_TLS
  printf("[TLS] %s()\n", __FUNCTION__);
#endif

  /* Consider only specific SSL packets (handshake) */
  if((packet->payload_packet_len < 17)
     || (packet->payload[0]  != 0x16)
     || (packet->payload[1]  != 0xfe) /* We ignore old DTLS versions */
     || ((packet->payload[2] != 0xff) && (packet->payload[2] != 0xfd))
     || ((ntohs(*((u_int16_t*)&packet->payload[11]))+13) != packet->payload_packet_len)
    ) {
  no_dtls:

#ifdef DEBUG_TLS
    printf("[TLS] No DTLS found\n");
#endif

    NDPI_EXCLUDE_PROTO(ndpi_struct, flow);
    return(0); /* Giveup */
  }

  // handshake_type = packet->payload[13];
  handshake_len  = (packet->payload[14] << 16) + (packet->payload[15] << 8) + packet->payload[16];

  if((handshake_len+25) != packet->payload_packet_len)
    goto no_dtls;

  /* Overwriting packet payload */
  p = packet->payload, p_len = packet->payload_packet_len; /* Backup */
  packet->payload = &packet->payload[13], packet->payload_packet_len -= 13;

  processTLSBlock(ndpi_struct, flow);

  packet->payload = p, packet->payload_packet_len = p_len; /* Restore */

  ndpi_int_tls_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_TLS);

  return(1); /* Keep working */
}