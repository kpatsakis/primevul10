int processCertificate(struct ndpi_detection_module_struct *ndpi_struct,
		       struct ndpi_flow_struct *flow) {
  struct ndpi_packet_struct *packet = &flow->packet;
  u_int32_t certificates_length, length = (packet->payload[1] << 16) + (packet->payload[2] << 8) + packet->payload[3];
  u_int16_t certificates_offset = 7;
  u_int8_t num_certificates_found = 0;

#ifdef DEBUG_TLS
  printf("[TLS] %s() [payload_packet_len=%u][direction: %u][%02X %02X %02X %02X %02X %02X...]\n",
	 __FUNCTION__, packet->payload_packet_len,
	 packet->packet_direction,
	 packet->payload[0], packet->payload[1], packet->payload[2],
	 packet->payload[3], packet->payload[4], packet->payload[5]);
#endif

  if((packet->payload_packet_len != (length + 4)) || (packet->payload[1] != 0x0))
    return(-1); /* Invalid length */

  certificates_length = (packet->payload[4] << 16) + (packet->payload[5] << 8) + packet->payload[6];

  if((packet->payload[4] != 0x0) || ((certificates_length+3) != length))
    return(-2); /* Invalid length */

  if(!flow->l4.tcp.tls.srv_cert_fingerprint_ctx) {
    if((flow->l4.tcp.tls.srv_cert_fingerprint_ctx = (void*)ndpi_malloc(sizeof(SHA1_CTX))) == NULL)
      return(-3); /* Not enough memory */
  }

  /* Now let's process each individual certificates */
  while(certificates_offset < certificates_length) {
    u_int32_t certificate_len = (packet->payload[certificates_offset] << 16) + (packet->payload[certificates_offset+1] << 8) + packet->payload[certificates_offset+2];

    /* Invalid lenght */
    if((certificate_len == 0)
       || (packet->payload[certificates_offset] != 0x0)
       || ((certificates_offset+certificate_len) > (4+certificates_length))) {
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

      SHA1Init(flow->l4.tcp.tls.srv_cert_fingerprint_ctx);

#ifdef DEBUG_CERTIFICATE_HASH
      {
	int i;

	for(i=0;i<certificate_len;i++)
	  printf("%02X ", packet->payload[certificates_offset+i]);

	printf("\n");
      }
#endif

      SHA1Update(flow->l4.tcp.tls.srv_cert_fingerprint_ctx,
		 &packet->payload[certificates_offset],
		 certificate_len);

      SHA1Final(flow->l4.tcp.tls.sha1_certificate_fingerprint, flow->l4.tcp.tls.srv_cert_fingerprint_ctx);

      flow->l4.tcp.tls.fingerprint_set = 1;

#ifdef DEBUG_TLS
      {
	int i;

	printf("[TLS] SHA-1: ");
	for(i=0;i<20;i++)
	  printf("%s%02X", (i > 0) ? ":" : "", flow->l4.tcp.tls.sha1_certificate_fingerprint[i]);
	printf("\n");
      }
#endif

      processCertificateElements(ndpi_struct, flow, certificates_offset, certificate_len);
    }

    certificates_offset += certificate_len;
  }

  flow->extra_packets_func = NULL; /* We're good now */
  return(1);
}