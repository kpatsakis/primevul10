int processCertificate(struct ndpi_detection_module_struct *ndpi_struct,
		       struct ndpi_flow_struct *flow) {
  struct ndpi_packet_struct *packet = &flow->packet;
  int is_dtls = packet->udp ? 1 : 0;
  u_int32_t certificates_length, length = (packet->payload[1] << 16) + (packet->payload[2] << 8) + packet->payload[3];
  u_int32_t certificates_offset = 7 + (is_dtls ? 8 : 0);
  u_int8_t num_certificates_found = 0;
  SHA1_CTX srv_cert_fingerprint_ctx ;

#ifdef DEBUG_TLS
  printf("[TLS] %s() [payload_packet_len=%u][direction: %u][%02X %02X %02X %02X %02X %02X...]\n",
	 __FUNCTION__, packet->payload_packet_len,
	 packet->packet_direction,
	 packet->payload[0], packet->payload[1], packet->payload[2],
	 packet->payload[3], packet->payload[4], packet->payload[5]);
#endif

  if((packet->payload_packet_len != (length + 4 + (is_dtls ? 8 : 0))) || (packet->payload[1] != 0x0)) {
    ndpi_set_risk(flow, NDPI_MALFORMED_PACKET);
    return(-1); /* Invalid length */
  }

  certificates_length = (packet->payload[certificates_offset - 3] << 16) +
                        (packet->payload[certificates_offset - 2] << 8) +
                        packet->payload[certificates_offset - 1];

  if((packet->payload[certificates_offset - 3] != 0x0) || ((certificates_length+3) != length)) {
    ndpi_set_risk(flow, NDPI_MALFORMED_PACKET);
    return(-2); /* Invalid length */
  }

  /* Now let's process each individual certificates */
  while(certificates_offset < certificates_length) {
    u_int32_t certificate_len = (packet->payload[certificates_offset] << 16) + (packet->payload[certificates_offset+1] << 8) + packet->payload[certificates_offset+2];

    /* Invalid lenght */
    if((certificate_len == 0)
       || (packet->payload[certificates_offset] != 0x0)
       || ((certificates_offset+certificate_len) > (4+certificates_length+(is_dtls ? 8 : 0)))) {
#ifdef DEBUG_TLS
      printf("[TLS] Invalid length [certificate_len: %u][certificates_offset: %u][%u vs %u]\n",
	     certificate_len, certificates_offset,
	     (certificates_offset+certificate_len),
	     certificates_length);
#endif
      break;
    }

    certificates_offset += 3;
#ifdef DEBUG_TLS
    printf("[TLS] Processing %u bytes certificate [%02X %02X %02X]\n",
	   certificate_len,
	   packet->payload[certificates_offset],
	   packet->payload[certificates_offset+1],
	   packet->payload[certificates_offset+2]);
#endif

    if(num_certificates_found++ == 0) /* Dissect only the first certificate that is the one we care */ {
      /* For SHA-1 we take into account only the first certificate and not all of them */

      SHA1Init(&srv_cert_fingerprint_ctx);

#ifdef DEBUG_CERTIFICATE_HASH
      {
	int i;

	for(i=0;i<certificate_len;i++)
	  printf("%02X ", packet->payload[certificates_offset+i]);

	printf("\n");
      }
#endif

      SHA1Update(&srv_cert_fingerprint_ctx,
		 &packet->payload[certificates_offset],
		 certificate_len);

      SHA1Final(flow->protos.tls_quic_stun.tls_quic.sha1_certificate_fingerprint, &srv_cert_fingerprint_ctx);

      flow->l4.tcp.tls.fingerprint_set = 1;

      uint8_t * sha1 = flow->protos.tls_quic_stun.tls_quic.sha1_certificate_fingerprint;
      const size_t sha1_siz = sizeof(flow->protos.tls_quic_stun.tls_quic.sha1_certificate_fingerprint);
      char sha1_str[20 /* sha1_siz */ * 2 + 1];
      static const char hexalnum[] = "0123456789ABCDEF";
      for (size_t i = 0; i < sha1_siz; ++i) {
        u_int8_t lower = (sha1[i] & 0x0F);
        u_int8_t upper = (sha1[i] & 0xF0) >> 4;
        sha1_str[i*2] = hexalnum[upper];
        sha1_str[i*2 + 1] = hexalnum[lower];
      }
      sha1_str[sha1_siz * 2] = '\0';

#ifdef DEBUG_TLS
      printf("[TLS] SHA-1: %s\n", sha1_str);
#endif

      if (ndpi_struct->malicious_sha1_automa.ac_automa != NULL) {
        u_int16_t rc1 = ndpi_match_string(ndpi_struct->malicious_sha1_automa.ac_automa, sha1_str);

        if(rc1 > 0)
          ndpi_set_risk(flow, NDPI_MALICIOUS_SHA1_CERTIFICATE);
      }

      processCertificateElements(ndpi_struct, flow, certificates_offset, certificate_len);
    }

    certificates_offset += certificate_len;
  }

  if((ndpi_struct->num_tls_blocks_to_follow != 0)
     && (flow->l4.tcp.tls.num_tls_blocks >= ndpi_struct->num_tls_blocks_to_follow)) {
#ifdef DEBUG_TLS_BLOCKS
    printf("*** [TLS Block] Enough blocks dissected\n");
#endif

    flow->extra_packets_func = NULL; /* We're good now */
  }

  return(1);
}