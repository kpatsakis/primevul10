snmp_ber_encode_oid(snmp_packet_t *snmp_packet, snmp_oid_t *oid)
{
  uint32_t val;
  uint16_t original_out_len;
  uint8_t pos;

  original_out_len = snmp_packet->used;

  pos = oid->length - 1;
  while(pos) {
    val = oid->data[pos];

    if(snmp_packet->used == snmp_packet->max) {
      return 0;
    }

    *snmp_packet->out-- = (uint8_t)(val & 0x7F);
    snmp_packet->used++;
    val >>= 7;

    while(val) {
      if(snmp_packet->used == snmp_packet->max) {
        return 0;
      }

      *snmp_packet->out-- = (uint8_t)((val & 0x7F) | 0x80);
      snmp_packet->used++;

      val >>= 7;
    }
    pos--;
  }

  if(snmp_packet->used == snmp_packet->max) {
    return 0;
  }

  val = *(snmp_packet->out + 1) + 40 * oid->data[pos];
  snmp_packet->used--;
  snmp_packet->out++;

  if(snmp_packet->used == snmp_packet->max) {
    return 0;
  }

  *snmp_packet->out-- = (uint8_t)(val & 0x7F);
  snmp_packet->used++;

  val >>= 7;

  while(val) {
    if(snmp_packet->used == snmp_packet->max) {
      return 0;
    }

    *snmp_packet->out-- = (uint8_t)((val & 0x7F) | 0x80);
    snmp_packet->used++;

    val >>= 7;
  }

  if(!snmp_ber_encode_length(snmp_packet, snmp_packet->used - original_out_len)) {
    return 0;
  }

  if(!snmp_ber_encode_type(snmp_packet, BER_DATA_TYPE_OBJECT_IDENTIFIER)) {
    return 0;
  }

  return 1;
}