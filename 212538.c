snmp_ber_decode_unsigned_integer(snmp_packet_t *snmp_packet, uint8_t expected_type, uint32_t *num)
{
  uint8_t i, len, type;

  if(!snmp_ber_decode_type(snmp_packet, &type)) {
    return 0;
  }

  if(type != expected_type) {
    /*
     * Sanity check
     * Invalid type in buffer
     */
    return 0;
  }

  if(!snmp_ber_decode_length(snmp_packet, &len)) {
    return 0;
  }

  if(len > 4) {
    /*
     * Sanity check
     * It will not fit in the uint32_t
     */
    return 0;
  }

  if(snmp_packet->used == 0) {
    return 0;
  }

  *num = (uint32_t)(*snmp_packet->in++ & 0xFF);
  snmp_packet->used--;

  for(i = 1; i < len; ++i) {
    *num <<= 8;
    if(snmp_packet->used == 0) {
      return 0;
    }
    *num |= (uint8_t)(*snmp_packet->in++ & 0xFF);
    snmp_packet->used--;
  }

  return 1;
}