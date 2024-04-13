snmp_ber_encode_string_len(snmp_packet_t *snmp_packet, const char *str, uint32_t length)
{
  uint32_t i;

  str += length - 1;
  for(i = 0; i < length; ++i) {
    if(snmp_packet->used == snmp_packet->max) {
      return 0;
    }

    *snmp_packet->out-- = (uint8_t)*str--;
    snmp_packet->used++;
  }

  if(!snmp_ber_encode_length(snmp_packet, length)) {
    return 0;
  }

  if(!snmp_ber_encode_type(snmp_packet, BER_DATA_TYPE_OCTET_STRING)) {
    return 0;
  }

  return 1;
}