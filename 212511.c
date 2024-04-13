snmp_engine(snmp_packet_t *snmp_packet)
{
  snmp_header_t header;
  snmp_varbind_t varbinds[SNMP_MAX_NR_VALUES];

  memset(&header, 0, sizeof(header));
  memset(varbinds, 0, sizeof(varbinds));

  if(!snmp_message_decode(snmp_packet, &header, varbinds)) {
    return 0;
  }

  if(header.version != SNMP_VERSION_1) {
    if(strncmp(header.community.community, SNMP_COMMUNITY, header.community.length)) {
      LOG_ERR("Request with invalid community\n");
      return 0;
    }
  }

  /*
   * Now handle the SNMP requests depending on their type
   */
  switch(header.pdu_type) {
  case BER_DATA_TYPE_PDU_GET_REQUEST:
    if(snmp_engine_get(&header, varbinds) == -1) {
      return 0;
    }
    break;

  case BER_DATA_TYPE_PDU_GET_NEXT_REQUEST:
    if(snmp_engine_get_next(&header, varbinds) == -1) {
      return 0;
    }
    break;

  case BER_DATA_TYPE_PDU_GET_BULK:
    if(snmp_engine_get_bulk(&header, varbinds) == -1) {
      return 0;
    }
    break;

  default:
    LOG_ERR("Invalid request type");
    return 0;
  }

  header.pdu_type = BER_DATA_TYPE_PDU_GET_RESPONSE;

  return snmp_message_encode(snmp_packet, &header, varbinds);
}