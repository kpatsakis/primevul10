static int ndpi_search_tls_tcp(struct ndpi_detection_module_struct *ndpi_struct,
			       struct ndpi_flow_struct *flow) {
  struct ndpi_packet_struct *packet = &flow->packet;
  u_int8_t something_went_wrong = 0;

#ifdef DEBUG_TLS_MEMORY
  printf("[TLS Mem] ndpi_search_tls_tcp() [payload_packet_len: %u]\n",
	 packet->payload_packet_len);
#endif

  if(packet->payload_packet_len == 0)
    return(1); /* Keep working */

  ndpi_search_tls_tcp_memory(ndpi_struct, flow);

  while(!something_went_wrong) {
    u_int16_t len, p_len;
    const u_int8_t *p;

    if(flow->l4.tcp.tls.message.buffer_used < 5)
      return(1); /* Keep working */

    len = (flow->l4.tcp.tls.message.buffer[3] << 8) + flow->l4.tcp.tls.message.buffer[4] + 5;

    if(len > flow->l4.tcp.tls.message.buffer_used) {
#ifdef DEBUG_TLS_MEMORY
      printf("[TLS Mem] Not enough TLS data [%u < %u][%02X %02X %02X %02X %02X]\n",
	     len, flow->l4.tcp.tls.message.buffer_used,
	     flow->l4.tcp.tls.message.buffer[0],
	     flow->l4.tcp.tls.message.buffer[1],
	     flow->l4.tcp.tls.message.buffer[2],
	     flow->l4.tcp.tls.message.buffer[3],
	     flow->l4.tcp.tls.message.buffer[4]);
#endif
      break;
    }

    if(len == 0) {
      something_went_wrong = 1;
      break;
    }

#ifdef DEBUG_TLS_MEMORY
    printf("[TLS Mem] Processing %u bytes message\n", len);
#endif

    /* Overwriting packet payload */
    p = packet->payload, p_len = packet->payload_packet_len; /* Backup */

    /* Split the element in blocks */
    u_int16_t processed = 5;

    while((processed+4) < len) {
      const u_int8_t *block = (const u_int8_t *)&flow->l4.tcp.tls.message.buffer[processed];
      u_int32_t block_len   = (block[1] << 16) + (block[2] << 8) + block[3];

      if((block_len == 0) || (block_len > len) || ((block[1] != 0x0))) {
	something_went_wrong = 1;
	break;
      }

      packet->payload = block, packet->payload_packet_len = ndpi_min(block_len+4, flow->l4.tcp.tls.message.buffer_used);

      if((processed+packet->payload_packet_len) > len) {
	something_went_wrong = 1;
	break;
      }

#ifdef DEBUG_TLS_MEMORY
      printf("*** [TLS Mem] Processing %u bytes block [%02X %02X %02X %02X %02X]\n",
	     packet->payload_packet_len,
	     packet->payload[0], packet->payload[1], packet->payload[2], packet->payload[3], packet->payload[4]);
#endif


      processTLSBlock(ndpi_struct, flow);
      processed += packet->payload_packet_len;
    }

    packet->payload = p, packet->payload_packet_len = p_len; /* Restore */
    flow->l4.tcp.tls.message.buffer_used -= len;

    if(flow->l4.tcp.tls.message.buffer_used > 0)
      memmove(flow->l4.tcp.tls.message.buffer,
	      &flow->l4.tcp.tls.message.buffer[len],
	      flow->l4.tcp.tls.message.buffer_used);
    else
      break;

#ifdef DEBUG_TLS_MEMORY
    printf("[TLS Mem] Left memory buffer %u bytes\n", flow->l4.tcp.tls.message.buffer_used);
#endif
  }

  if(something_went_wrong) {
    flow->check_extra_packets = 0, flow->extra_packets_func = NULL;
    return(0); /* That's all */
  } else
    return(1);
}