snmp_ber_encode_integer(snmp_packet_t *snmp_packet, uint32_t number)
{
  return snmp_ber_encode_unsigned_integer(snmp_packet, BER_DATA_TYPE_INTEGER, number);
}