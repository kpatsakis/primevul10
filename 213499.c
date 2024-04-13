static int ndpi_search_tls_udp(struct ndpi_detection_module_struct *ndpi_struct,
			       struct ndpi_flow_struct *flow) {
  struct ndpi_packet_struct *packet = &flow->packet;
  u_int32_t handshake_len;
  u_int16_t p_len, processed;
  const u_int8_t *p;
  u_int8_t no_dtls = 0, change_cipher_found = 0;

#ifdef DEBUG_TLS
  printf("[TLS] %s()\n", __FUNCTION__);
#endif

  /* Overwriting packet payload */
  p = packet->payload, p_len = packet->payload_packet_len; /* Backup */

  /* Split the element in blocks */
  processed = 0;
  while(processed + 13 < p_len) {
    u_int32_t block_len;
    const u_int8_t *block = (const u_int8_t *)&p[processed];

    if((block[0] != 0x16 && block[0] != 0x14) || /* Handshake, change-cipher-spec */
       (block[1] != 0xfe) || /* We ignore old DTLS versions */
       ((block[2] != 0xff) && (block[2] != 0xfd))) {
#ifdef DEBUG_TLS
      printf("[TLS] DTLS invalid block 0x%x or old version 0x%x-0x%x-0x%x\n",
             block[0], block[1], block[2], block[3]);
#endif
      no_dtls = 1;
      break;
    }
    block_len = ntohs(*((u_int16_t*)&block[11]));
#ifdef DEBUG_TLS
    printf("[TLS] DTLS block len: %d\n", block_len);
#endif
    if (block_len == 0 || (processed + block_len + 12 >= p_len)) {
#ifdef DEBUG_TLS
      printf("[TLS] DTLS invalid block len %d (processed %d, p_len %d)\n",
             block_len, processed, p_len);
#endif
      no_dtls = 1;
      break;
    }
    /* We process only handshake msgs */
    if(block[0] == 0x16) {
      if (processed + block_len + 13 > p_len) {
#ifdef DEBUG_TLS
        printf("[TLS] DTLS invalid len %d %d %d\n", processed, block_len, p_len);
#endif
        no_dtls = 1;
        break;
     }
      /* TODO: handle (certificate) fragments */
      handshake_len = (block[14] << 16) + (block[15] << 8) + block[16];
      if((handshake_len + 12) != block_len) {
#ifdef DEBUG_TLS
        printf("[TLS] DTLS invalid handshake_len %d, %d)\n",
               handshake_len, block_len);
#endif
        no_dtls = 1;
        break;
      }
      packet->payload = &block[13];
      packet->payload_packet_len = block_len;
      processTLSBlock(ndpi_struct, flow);
    } else {
      /* Change-cipher-spec: any subsequent block might be encrypted */
#ifdef DEBUG_TLS
      printf("[TLS] Change-cipher-spec\n");
#endif
      change_cipher_found = 1;
      processed += block_len + 13;
      break;
    }

    processed += block_len + 13;
  }
  if(processed != p_len) {
#ifdef DEBUG_TLS
    printf("[TLS] DTLS invalid processed len %d/%d (%d)\n", processed, p_len, change_cipher_found);
#endif
    if(!change_cipher_found)
      no_dtls = 1;
  }

  packet->payload = p;
  packet->payload_packet_len = p_len; /* Restore */

  if(no_dtls || change_cipher_found) {
    NDPI_EXCLUDE_PROTO(ndpi_struct, flow);
    return(0); /* That's all */
  } else {
    return(1); /* Keep working */
  }
}