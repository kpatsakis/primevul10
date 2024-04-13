static void node_print_unknown_proto_walker(const void *node,
					    ndpi_VISIT which, int depth, void *user_data) {
  struct ndpi_flow_info *flow = *(struct ndpi_flow_info**)node;
  u_int16_t thread_id = *((u_int16_t*)user_data);

  if((flow->detected_protocol.master_protocol != NDPI_PROTOCOL_UNKNOWN)
     || (flow->detected_protocol.app_protocol != NDPI_PROTOCOL_UNKNOWN))
    return;

  if((which == ndpi_preorder) || (which == ndpi_leaf)) {
    /* Avoid walking the same node multiple times */
    all_flows[num_flows].thread_id = thread_id, all_flows[num_flows].flow = flow;
    num_flows++;
  }
}