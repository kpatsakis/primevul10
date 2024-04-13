snmp_ber_encode_unsigned_integer(snmp_packet_t *snmp_packet, uint8_t type, uint32_t number)
{
  uint16_t original_out_len;

  original_out_len = snmp_packet->used;
  do {
    if(snmp_packet->used == snmp_packet->max) {
      return 0;
    }

    *snmp_packet->out-- = (uint8_t)number & 0xFF;
    snmp_packet->used++;
    /* I'm not sure why but on MSPGCC the >> 8 operation goes haywire here */
#ifdef __MSPGCC__
    number >>= 4;
    number >>= 4;
#else /* __MSPGCC__ */
    number >>= 8;
#endif /* __MSPGCC__ */
  } while(number);

  if(!snmp_ber_encode_length(snmp_packet, snmp_packet->used - original_out_len)) {
    return 0;
  }

  if(!snmp_ber_encode_type(snmp_packet, type)) {
    return 0;
  }

  return 1;
}