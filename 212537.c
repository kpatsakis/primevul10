snmp_ber_encode_type(snmp_packet_t *snmp_packet, uint8_t type)
{
  if(snmp_packet->used == snmp_packet->max) {
    return 0;
  }

  *snmp_packet->out-- = type;
  snmp_packet->used++;

  return 1;
}