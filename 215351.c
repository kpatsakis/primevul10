static int ndpi_check_protocol_port_mismatch_exceptions(struct ndpi_detection_module_struct *ndpi_str,
							struct ndpi_flow_struct *flow,
							ndpi_default_ports_tree_node_t *expected_proto,
							ndpi_protocol *returned_proto) {
  /*
    For TLS (and other protocols) it is not simple to guess the exact protocol so before
    triggering an alert we need to make sure what we have exhausted all the possible
    options available
  */

  if(returned_proto->master_protocol == NDPI_PROTOCOL_TLS) {
    switch(expected_proto->proto->protoId) {
    case NDPI_PROTOCOL_MAIL_IMAPS:
    case NDPI_PROTOCOL_MAIL_POPS:
    case NDPI_PROTOCOL_MAIL_SMTPS:
      return(1); /* This is a reasonable exception */
      break;
    }
  }

  return(0);
}