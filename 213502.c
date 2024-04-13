static void ndpi_looks_like_tls(struct ndpi_detection_module_struct *ndpi_struct,
				struct ndpi_flow_struct *flow) {
  // ndpi_set_detected_protocol(ndpi_struct, flow, NDPI_PROTOCOL_TLS, NDPI_PROTOCOL_UNKNOWN);

  if(flow->guessed_protocol_id == NDPI_PROTOCOL_UNKNOWN)
    flow->guessed_protocol_id = NDPI_PROTOCOL_TLS;
}