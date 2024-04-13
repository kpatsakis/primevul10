snmp_ber_encode_null(snmp_packet_t *snmp_packet, uint8_t type)
{

  if(snmp_packet->used == snmp_packet->max) {
    return 0;
  }

  *snmp_packet->out-- = 0x00;
  snmp_packet->used++;

  return snmp_ber_encode_type(snmp_packet, type);
}