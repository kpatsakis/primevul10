snmp_ber_decode_null(snmp_packet_t *snmp_packet)
{
  if(snmp_packet->used == 0) {
    return 0;
  }

  snmp_packet->in++;
  snmp_packet->used--;

  if(snmp_packet->used == 0) {
    return 0;
  }

  snmp_packet->in++;
  snmp_packet->used--;

  return 1;
}