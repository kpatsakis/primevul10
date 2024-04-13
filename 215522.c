static void tlsInitExtraPacketProcessing(struct ndpi_flow_struct *flow) {
  flow->check_extra_packets = 1;

  /* At most 12 packets should almost always be enough to find the server certificate if it's there */
  flow->max_extra_packets_to_check = 12;
  flow->extra_packets_func = (flow->packet.udp != NULL) ? ndpi_search_tls_udp : ndpi_search_tls_tcp;
}