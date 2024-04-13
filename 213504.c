void ndpi_search_tls_tcp_memory(struct ndpi_detection_module_struct *ndpi_struct,
				struct ndpi_flow_struct *flow) {
  struct ndpi_packet_struct *packet = &flow->packet;
  u_int avail_bytes;

  /* TCP */
#ifdef DEBUG_TLS_MEMORY
  printf("[TLS Mem] Handling TCP/TLS flow [payload_len: %u][buffer_len: %u][direction: %u]\n",
	 packet->payload_packet_len,
	 flow->l4.tcp.tls.message.buffer_len,
	 packet->packet_direction);
#endif

  if(flow->l4.tcp.tls.message.buffer == NULL) {
    /* Allocate buffer */
    flow->l4.tcp.tls.message.buffer_len = 2048, flow->l4.tcp.tls.message.buffer_used = 0;
    flow->l4.tcp.tls.message.buffer = (u_int8_t*)ndpi_malloc(flow->l4.tcp.tls.message.buffer_len);

    if(flow->l4.tcp.tls.message.buffer == NULL)
      return;

#ifdef DEBUG_TLS_MEMORY
    printf("[TLS Mem] Allocating %u buffer\n", flow->l4.tcp.tls.message.buffer_len);
#endif
  }

  avail_bytes = flow->l4.tcp.tls.message.buffer_len - flow->l4.tcp.tls.message.buffer_used;

  if(avail_bytes < packet->payload_packet_len) {
    u_int new_len = flow->l4.tcp.tls.message.buffer_len + packet->payload_packet_len - avail_bytes + 1;
    void *newbuf  = ndpi_realloc(flow->l4.tcp.tls.message.buffer,
				 flow->l4.tcp.tls.message.buffer_len, new_len);
    if(!newbuf) return;

#ifdef DEBUG_TLS_MEMORY
    printf("[TLS Mem] Enlarging %u -> %u buffer\n", flow->l4.tcp.tls.message.buffer_len, new_len);
#endif

    flow->l4.tcp.tls.message.buffer = (u_int8_t*)newbuf;
    flow->l4.tcp.tls.message.buffer_len = new_len;
    avail_bytes = flow->l4.tcp.tls.message.buffer_len - flow->l4.tcp.tls.message.buffer_used;
  }

  if(packet->payload_packet_len > 0 && avail_bytes >= packet->payload_packet_len) {
    u_int8_t ok = 0;

    if(flow->l4.tcp.tls.message.next_seq[packet->packet_direction] != 0) {
      if(ntohl(packet->tcp->seq) == flow->l4.tcp.tls.message.next_seq[packet->packet_direction])
	ok = 1;
    } else
      ok = 1;

    if(ok) {
      memcpy(&flow->l4.tcp.tls.message.buffer[flow->l4.tcp.tls.message.buffer_used],
	     packet->payload, packet->payload_packet_len);

      flow->l4.tcp.tls.message.buffer_used += packet->payload_packet_len;
#ifdef DEBUG_TLS_MEMORY
      printf("[TLS Mem] Copied data to buffer [%u/%u bytes][direction: %u][tcp_seq: %u][next: %u]\n",
	     flow->l4.tcp.tls.message.buffer_used, flow->l4.tcp.tls.message.buffer_len,
	     packet->packet_direction,
	     ntohl(packet->tcp->seq),
	     ntohl(packet->tcp->seq)+packet->payload_packet_len);
#endif

      flow->l4.tcp.tls.message.next_seq[packet->packet_direction] = ntohl(packet->tcp->seq)+packet->payload_packet_len;
    } else {
#ifdef DEBUG_TLS_MEMORY
      printf("[TLS Mem] Skipping packet [%u bytes][direction: %u][tcp_seq: %u][expected next: %u]\n",
	     flow->l4.tcp.tls.message.buffer_len,
	     packet->packet_direction,
	     ntohl(packet->tcp->seq),
	     ntohl(packet->tcp->seq)+packet->payload_packet_len);
#endif
    }
  }
}