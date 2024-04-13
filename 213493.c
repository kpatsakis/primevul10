void ndpi_search_netbios(struct ndpi_detection_module_struct *ndpi_struct,
			 struct ndpi_flow_struct *flow) {
  struct ndpi_packet_struct *packet = &flow->packet;
  u_int16_t dport;

  NDPI_LOG_DBG(ndpi_struct, "search netbios\n");

  if(packet->udp != NULL) {
    dport = ntohs(packet->udp->dest);

    /*check standard NETBIOS over udp to port 137  */
    if((dport == 137 || 0) && packet->payload_packet_len >= 50) {
      struct netbios_header h;

      memcpy(&h, packet->payload, sizeof(struct netbios_header));
      h.transaction_id = ntohs(h.transaction_id), h.flags = ntohs(h.flags),
	h.questions = ntohs(h.questions), h.answer_rrs = ntohs(h.answer_rrs),
	h.authority_rrs = ntohs(h.authority_rrs), h.additional_rrs = ntohs(h.additional_rrs);

      NDPI_LOG_DBG(ndpi_struct, "found netbios port 137 and payload_packet_len 50\n");

      if(h.flags == 0 &&
	 h.questions == 1 &&
	 h.answer_rrs == 0 &&
	 h.authority_rrs == 0 && h.additional_rrs == 0) {

	NDPI_LOG_INFO(ndpi_struct, "found netbios with questions = 1 and answers = 0, authority = 0  \n");

	ndpi_int_netbios_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_UNKNOWN);
	return;
      }

      if(((h.flags & 0x8710) == 0x10) &&
	 h.questions == 1 &&
	 h.answer_rrs == 0 &&
	 h.authority_rrs == 0) {

	NDPI_LOG_INFO(ndpi_struct, "found netbios with questions = 1 and answers = 0, authority = 0 and broadcast \n");

	ndpi_int_netbios_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_UNKNOWN);
	return;
      }

      if(packet->payload[2] == 0x80 &&
	 h.questions == 1 &&
	 h.answer_rrs == 0 &&
	 h.authority_rrs == 0 && h.additional_rrs == 1) {

	NDPI_LOG_INFO(ndpi_struct, "found netbios with questions = 1 and answers, authority, additional = 0  \n");

	ndpi_int_netbios_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_UNKNOWN);
	return;
      }

      if(h.flags == 0x4000 &&
	 h.questions == 1 &&
	 h.answer_rrs == 0 &&
	 h.authority_rrs == 0 && h.additional_rrs == 1) {

	NDPI_LOG_INFO(ndpi_struct, "found netbios with questions = 1 and answers = 0, authority = 0  \n");

	ndpi_int_netbios_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_UNKNOWN);
	return;
      }

      if(h.flags == 0x8400 &&
	 h.questions == 0 &&
	 h.answer_rrs == 1 &&
	 h.authority_rrs == 0 && h.additional_rrs == 0) {

	NDPI_LOG_INFO(ndpi_struct,
		      "found netbios with flag 8400 questions = 0 and answers = 1, authority, additional = 0  \n");

	ndpi_int_netbios_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_UNKNOWN);
	return;
      }

      if(h.flags == 0x8500 &&
	 h.questions == 0 &&
	 h.answer_rrs == 1 &&
	 h.authority_rrs == 0 && h.additional_rrs == 0) {

	NDPI_LOG_INFO(ndpi_struct,
		      "found netbios with flag 8500 questions = 0 and answers = 1, authority, additional = 0  \n");

	ndpi_int_netbios_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_UNKNOWN);
	return;
      }

      if(((h.flags == 0x2900) || (h.flags == 0x2910)) &&
	 h.questions == 1 &&
	 h.answer_rrs == 0 &&
	 h.authority_rrs == 0 && h.additional_rrs == 1) {

	NDPI_LOG_INFO(ndpi_struct,
		      "found netbios with flag 2910, questions = 1 and answers, authority=0, additional = 1  \n");

	ndpi_int_netbios_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_UNKNOWN);
	return;
      }

      if(h.flags == 0xAD86 &&
	 h.questions == 0 &&
	 h.answer_rrs == 1 &&
	 h.authority_rrs == 0 && h.additional_rrs == 0) {

	NDPI_LOG_INFO(ndpi_struct,
		      "found netbios with flag ad86 questions = 0 and answers = 1, authority, additional = 0  \n");

	ndpi_int_netbios_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_UNKNOWN);
	return;
      }

      if(h.flags == 0x0110 &&
	 h.questions == 1 &&
	 h.answer_rrs == 0 &&
	 h.authority_rrs == 0 && h.additional_rrs == 0) {

	NDPI_LOG_INFO(ndpi_struct,
		      "found netbios with flag 0110 questions = 1 and answers = 0, authority, additional = 0  \n");

	ndpi_int_netbios_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_UNKNOWN);
	return;
      }

      if((h.flags & 0xf800) == 0) {
	NDPI_LOG_DBG2(ndpi_struct, "possible netbios name query request\n");

	if(get_u_int16_t(packet->payload, 4) == htons(1) &&
	   get_u_int16_t(packet->payload, 6) == 0 &&
	   get_u_int16_t(packet->payload, 8) == 0 && get_u_int16_t(packet->payload, 10) == 0) {

	  /* name is encoded as described in rfc883 */
	  u_int8_t name_length = packet->payload[12];

	  NDPI_LOG_DBG2(ndpi_struct,
			"possible netbios name query request, one question\n");

	  if(packet->payload_packet_len == 12 + 1 + name_length + 1 + 2 + 2) {

	    NDPI_LOG_DBG2(ndpi_struct,
			  "possible netbios name query request, length matches\n");

	    /* null terminated? */
	    if(packet->payload[12 + name_length + 1] == 0 &&
	       get_u_int16_t(packet->payload, 12 + name_length + 2) == htons(0x0020) &&
	       get_u_int16_t(packet->payload, 12 + name_length + 4) == htons(0x0001)) {

	      NDPI_LOG_INFO(ndpi_struct,
			    "found netbios name query request\n");
	      ndpi_int_netbios_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_UNKNOWN);
	      return;
	    }
	  }
	}
      } else if((h.flags & 0xf800) == 0x8000) {
	NDPI_LOG_DBG2(ndpi_struct,
		      "possible netbios name query response\n");

	if(get_u_int16_t(packet->payload, 4) == 0 &&
	   get_u_int16_t(packet->payload, 6) == htons(1) &&
	   get_u_int16_t(packet->payload, 8) == 0 && get_u_int16_t(packet->payload, 10) == 0) {

	  /* name is encoded as described in rfc883 */
	  u_int8_t name_length = packet->payload[12];

	  NDPI_LOG_DBG2(ndpi_struct,
			"possible netbios positive name query response, one answer\n");

	  if(packet->payload_packet_len >= 12 + 1 + name_length + 1 + 2 + 2) {

	    NDPI_LOG_DBG2(ndpi_struct,
			  "possible netbios name query response, length matches\n");

	    /* null terminated? */
	    if(packet->payload[12 + name_length + 1] == 0 &&
	       get_u_int16_t(packet->payload, 12 + name_length + 2) == htons(0x0020) &&
	       get_u_int16_t(packet->payload, 12 + name_length + 4) == htons(0x0001)) {

	      NDPI_LOG_INFO(ndpi_struct,
			    "found netbios name query response\n");
	      ndpi_int_netbios_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_UNKNOWN);
	      return;
	    }
	  }
	} else if(get_u_int16_t(packet->payload, 4) == 0 &&
		  get_u_int16_t(packet->payload, 6) == 0 &&
		  get_u_int16_t(packet->payload, 8) == 0 && get_u_int16_t(packet->payload, 10) == 0) {

	  /* name is encoded as described in rfc883 */
	  u_int8_t name_length = packet->payload[12];

	  NDPI_LOG_DBG2(ndpi_struct,
			"possible netbios negative name query response, one answer\n");

	  if(packet->payload_packet_len >= 12 + 1 + name_length + 1 + 2 + 2) {

	    NDPI_LOG_DBG2(ndpi_struct,
			  "possible netbios name query response, length matches\n");

	    /* null terminated? */
	    if(packet->payload[12 + name_length + 1] == 0 &&
	       get_u_int16_t(packet->payload, 12 + name_length + 2) == htons(0x000A) &&
	       get_u_int16_t(packet->payload, 12 + name_length + 4) == htons(0x0001)) {

	      NDPI_LOG_INFO(ndpi_struct,
			    "found netbios name query response\n");
	      ndpi_int_netbios_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_UNKNOWN);
	      return;
	    }
	  }
	} else if(get_u_int16_t(packet->payload, 4) == 0 &&
		  get_u_int16_t(packet->payload, 6) == 0 &&
		  get_u_int16_t(packet->payload, 8) == htons(1) && get_u_int16_t(packet->payload, 10) == htons(1)) {

	  /* name is encoded as described in rfc883 */
	  u_int8_t name_length = packet->payload[12];

	  NDPI_LOG_DBG2(ndpi_struct,
			"possible netbios redirect name query response, one answer\n");

	  if(packet->payload_packet_len >= 12 + 1 + name_length + 1 + 2 + 2) {

	    NDPI_LOG_DBG2(ndpi_struct,
			  "possible netbios name query response, length matches\n");

	    /* null terminated? */
	    if(packet->payload[12 + name_length + 1] == 0 &&
	       get_u_int16_t(packet->payload, 12 + name_length + 2) == htons(0x0002) &&
	       get_u_int16_t(packet->payload, 12 + name_length + 4) == htons(0x0001)) {

	      NDPI_LOG_INFO(ndpi_struct,
			    "found netbios name query response\n");
	      ndpi_int_netbios_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_UNKNOWN);
	      return;
	    }
	  }
	}
      }
      /* TODO: extend according to rfc1002 */
    }

    /* check standard NETBIOS over udp to port 138 */

    /* netbios header token from http://www.protocolbase.net/protocols/protocol_NBDGM.php */

    if((dport == 138) && (packet->payload_packet_len >= 14)) {
      u_int16_t netbios_len = ntohs(get_u_int16_t(packet->payload, 10));

      if(netbios_len == packet->payload_packet_len - 14) {
	NDPI_LOG_DBG2(ndpi_struct, "found netbios port 138 and payload length >= 112 \n");

	if(packet->payload[0] >= 0x10 && packet->payload[0] <= 0x16) {
	  u_int32_t source_ip = ntohl(get_u_int32_t(packet->payload, 4));

	  NDPI_LOG_DBG2(ndpi_struct, "found netbios with MSG-type 0x10,0x11,0x12,0x13,0x14,0x15 or 0x16\n");

	  if(source_ip == ntohl(packet->iph->saddr)) {
	    int16_t leftover = netbios_len - 82; /* NetBIOS len */

	    NDPI_LOG_INFO(ndpi_struct, "found netbios with checked ip-address\n");

	    ndpi_int_netbios_add_connection(ndpi_struct, flow, (leftover > 0) ? NDPI_PROTOCOL_SMBV1 : NDPI_PROTOCOL_UNKNOWN);
	    return;
	  }
	}
      }
    }
  }

  if(packet->tcp != NULL) {
    dport = ntohs(packet->tcp->dest);

    /* destination port must be 139 */
    if(dport == 139) {
      NDPI_LOG_DBG2(ndpi_struct, "found netbios with destination port 139\n");

      /* payload_packet_len must be 72 */
      if(packet->payload_packet_len == 72) {
	NDPI_LOG_DBG2(ndpi_struct, "found netbios with payload_packen_len = 72. \n");

	if(packet->payload[0] == 0x81 && packet->payload[1] == 0 && ntohs(get_u_int16_t(packet->payload, 2)) == 68) {
	  NDPI_LOG_INFO(ndpi_struct,
			"found netbios with session request = 81, flags=0 and length od following bytes = 68. \n");

	  ndpi_int_netbios_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_UNKNOWN);
	  return;
	}
      }
    }

  }

  NDPI_EXCLUDE_PROTO(ndpi_struct, flow);
}