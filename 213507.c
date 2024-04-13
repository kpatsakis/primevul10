static int processTLSBlock(struct ndpi_detection_module_struct *ndpi_struct,
			   struct ndpi_flow_struct *flow) {
  struct ndpi_packet_struct *packet = &flow->packet;
  int ret;

  switch(packet->payload[0] /* block type */) {
  case 0x01: /* Client Hello */
  case 0x02: /* Server Hello */
    processClientServerHello(ndpi_struct, flow, 0);
    flow->l4.tcp.tls.hello_processed = 1;
    ndpi_int_tls_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_TLS);

#ifdef DEBUG_TLS
    printf("*** TLS [version: %02X][%s Hello]\n",
	   flow->protos.tls_quic_stun.tls_quic.ssl_version,
	   (packet->payload[0] == 0x01) ? "Client" : "Server");
#endif

    if((flow->protos.tls_quic_stun.tls_quic.ssl_version >= 0x0304 /* TLS 1.3 */)
       && (packet->payload[0] == 0x02 /* Server Hello */)) {
      flow->l4.tcp.tls.certificate_processed = 1; /* No Certificate with TLS 1.3+ */
    }

    checkTLSSubprotocol(ndpi_struct, flow);
    break;

  case 0x0b: /* Certificate */
    /* Important: populate the tls union fields only after
     * ndpi_int_tls_add_connection has been called */
    if(flow->l4.tcp.tls.hello_processed) {
      ret = processCertificate(ndpi_struct, flow);
      if (ret != 1) {
#ifdef DEBUG_TLS
        printf("[TLS] Error processing certificate: %d\n", ret);
#endif
      }
      flow->l4.tcp.tls.certificate_processed = 1;
    }
    break;

  default:
    return(-1);
  }

  return(0);
}