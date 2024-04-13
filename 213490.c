static void checkTLSSubprotocol(struct ndpi_detection_module_struct *ndpi_struct,
				struct ndpi_flow_struct *flow) {
  if(flow->detected_protocol_stack[1] == NDPI_PROTOCOL_UNKNOWN) {
    /* Subprotocol not yet set */

    if(ndpi_struct->tls_cert_cache && flow->packet.iph) {
      u_int32_t key = flow->packet.iph->daddr + flow->packet.tcp->dest;
      u_int16_t cached_proto;

      if(ndpi_lru_find_cache(ndpi_struct->tls_cert_cache, key,
			     &cached_proto, 0 /* Don't remove it as it can be used for other connections */)) {
	ndpi_protocol ret = { NDPI_PROTOCOL_TLS, cached_proto, NDPI_PROTOCOL_CATEGORY_UNSPECIFIED };

	flow->detected_protocol_stack[0] = cached_proto,
	flow->detected_protocol_stack[1] = NDPI_PROTOCOL_TLS;

	flow->category = ndpi_get_proto_category(ndpi_struct, ret);
	ndpi_check_subprotocol_risk(flow, cached_proto);
      }
    }
  }
}