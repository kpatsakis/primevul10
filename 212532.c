snmp_message_decode(snmp_packet_t *snmp_packet, snmp_header_t *header, snmp_varbind_t *varbinds)
{
  uint8_t type, len, i;

  if(!snmp_ber_decode_type(snmp_packet, &type)) {
    LOG_DBG("Could not decode type\n");
    return 0;
  }

  if(type != BER_DATA_TYPE_SEQUENCE) {
    LOG_DBG("Invalid type\n");
    return 0;
  }

  if(!snmp_ber_decode_length(snmp_packet, &len)) {
    LOG_DBG("Could not decode length\n");
    return 0;
  }

  if(!snmp_ber_decode_integer(snmp_packet, &header->version)) {
    LOG_DBG("Could not decode version\n");
    return 0;
  }

  switch(header->version) {
  case SNMP_VERSION_1:
  case SNMP_VERSION_2C:
    break;
  default:
    LOG_DBG("Invalid version\n");
    return 0;
  }

  if(!snmp_ber_decode_string_len_buffer(snmp_packet, &header->community.community, &header->community.length)) {
    LOG_DBG("Could not decode community\n");
    return 0;
  }

  if(!snmp_ber_decode_type(snmp_packet, &header->pdu_type)) {
    LOG_DBG("Could not decode pdu type\n");
    return 0;
  }

  switch(header->pdu_type) {
  case BER_DATA_TYPE_PDU_GET_REQUEST:
  case BER_DATA_TYPE_PDU_GET_NEXT_REQUEST:
  case BER_DATA_TYPE_PDU_GET_RESPONSE:
  case BER_DATA_TYPE_PDU_SET_REQUEST:
  case BER_DATA_TYPE_PDU_GET_BULK:
    break;
  default:
    LOG_DBG("Invalid version\n");
    return 0;
  }

  if(!snmp_ber_decode_length(snmp_packet, &len)) {
    LOG_DBG("Could not decode length\n");
    return 0;
  }

  if(!snmp_ber_decode_integer(snmp_packet, &header->request_id)) {
    LOG_DBG("Could not decode request id\n");
    return 0;
  }

  switch(header->pdu_type) {
  case BER_DATA_TYPE_PDU_GET_BULK:
    if(!snmp_ber_decode_integer(snmp_packet, &header->non_repeaters)) {
      LOG_DBG("Could not decode non repeaters\n");
      return 0;
    }

    if(!snmp_ber_decode_integer(snmp_packet, &header->max_repetitions)) {
      LOG_DBG("Could not decode max repetition\n");
      return 0;
    }
    break;
  default:
    if(!snmp_ber_decode_integer(snmp_packet, &header->error_status)) {
      LOG_DBG("Could not decode error status\n");
      return 0;
    }

    if(!snmp_ber_decode_integer(snmp_packet, &header->error_index)) {
      LOG_DBG("Could not decode error index\n");
      return 0;
    }
    break;
  }

  if(!snmp_ber_decode_type(snmp_packet, &type)) {
    LOG_DBG("Could not decode type\n");
    return 0;
  }

  if(type != BER_DATA_TYPE_SEQUENCE) {
    LOG_DBG("Invalid type\n");
    return 0;
  }

  if(!snmp_ber_decode_length(snmp_packet, &len)) {
    LOG_DBG("Could not decode length\n");
    return 0;
  }

  for(i = 0; snmp_packet->used > 0; ++i) {
    if(i >= SNMP_MAX_NR_VALUES) {
      LOG_DBG("OID's overflow\n");
      return 0;
    }

    if(!snmp_ber_decode_type(snmp_packet, &type)) {
      LOG_DBG("Could not decode type\n");
      return 0;
    }

    if(type != BER_DATA_TYPE_SEQUENCE) {
      LOG_DBG("Invalid (%X) type\n", type);
      return 0;
    }

    if(!snmp_ber_decode_length(snmp_packet, &len)) {
      LOG_DBG("Could not decode length\n");
      return 0;
    }

    if(!snmp_ber_decode_oid(snmp_packet, &varbinds[i].oid)) {
      LOG_DBG("Could not decode oid\n");
      return 0;
    }

    varbinds[i].value_type = *snmp_packet->in;

    switch(varbinds[i].value_type) {
    case BER_DATA_TYPE_INTEGER:
      if(!snmp_ber_decode_integer(snmp_packet, &varbinds[i].value.integer)) {
        LOG_DBG("Could not decode integer type\n");
        return 0;
      }
      break;
    case BER_DATA_TYPE_TIMETICKS:
      if(!snmp_ber_decode_timeticks(snmp_packet, &varbinds[i].value.integer)) {
        LOG_DBG("Could not decode timeticks type\n");
        return 0;
      }
      break;
    case BER_DATA_TYPE_OCTET_STRING:
      if(!snmp_ber_decode_string_len_buffer(snmp_packet, &varbinds[i].value.string.string, &varbinds[i].value.string.length)) {
        LOG_DBG("Could not decode octed string type\n");
        return 0;
      }
      break;
    case BER_DATA_TYPE_NULL:
      if(!snmp_ber_decode_null(snmp_packet)) {
        LOG_DBG("Could not decode null type\n");
        return 0;
      }
      break;
    default:
      LOG_DBG("Invalid varbind type\n");
      return 0;
    }
  }

  return 1;
}