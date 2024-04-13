snmp_ber_decode_oid(snmp_packet_t *snmp_packet, snmp_oid_t *oid)
{
  uint8_t *buf_end, type;
  uint8_t len, j;
  div_t first;

  if(!snmp_ber_decode_type(snmp_packet, &type)) {
    return 0;
  }

  if(type != BER_DATA_TYPE_OBJECT_IDENTIFIER) {
    return 0;
  }

  if(!snmp_ber_decode_length(snmp_packet, &len)) {
    return 0;
  }

  buf_end = snmp_packet->in + len;

  if(snmp_packet->used == 0) {
    return 0;
  }

  snmp_packet->used--;
  first = div(*snmp_packet->in++, 40);

  oid->length = 0;

  oid->data[oid->length++] = (uint32_t)first.quot;
  oid->data[oid->length++] = (uint32_t)first.rem;

  while(snmp_packet->in != buf_end) {
    if(oid->length >= SNMP_MSG_OID_MAX_LEN) {
      return 0;
    }

    if(snmp_packet->used == 0) {
      return 0;
    }
    oid->data[oid->length] = (uint32_t)(*snmp_packet->in & 0x7F);
    for(j = 0; j < 4; j++) {
      snmp_packet->used--;
      if((*snmp_packet->in++ & 0x80) == 0) {
        break;
      }

      if(snmp_packet->used == 0) {
        return 0;
      }

      oid->data[oid->length] <<= 7;
      oid->data[oid->length] |= (*snmp_packet->in & 0x7F);
    }

    oid->length++;
  }

  return 1;
}