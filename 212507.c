snmp_ber_decode_type(snmp_packet_t *snmp_packet, uint8_t *type)
{
  if(snmp_packet->used == 0) {
    return 0;
  }

  *type = *snmp_packet->in++;
  snmp_packet->used--;

  return 1;
}