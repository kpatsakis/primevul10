snmp_ber_decode_timeticks(snmp_packet_t *snmp_packet, uint32_t *timeticks)
{
  return snmp_ber_decode_unsigned_integer(snmp_packet, BER_DATA_TYPE_TIMETICKS, timeticks);
}