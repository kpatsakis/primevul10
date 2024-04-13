snmp_ber_decode_integer(snmp_packet_t *snmp_packet, uint32_t *num)
{
  return snmp_ber_decode_unsigned_integer(snmp_packet, BER_DATA_TYPE_INTEGER, num);
}