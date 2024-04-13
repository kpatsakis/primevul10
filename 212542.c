snmp_ber_encode_length(snmp_packet_t *snmp_packet, uint16_t length)
{
  if(length > 0xFF) {
    if(snmp_packet->used == snmp_packet->max) {
      return 0;
    }

    *snmp_packet->out-- = (uint8_t)length & 0xFF;
    snmp_packet->used++;

    if(snmp_packet->used == snmp_packet->max) {
      return 0;
    }

    *snmp_packet->out-- = (uint8_t)(length >> 8) & 0xFF;
    snmp_packet->used++;

    if(snmp_packet->used == snmp_packet->max) {
      return 0;
    }

    *snmp_packet->out-- = 0x82;
    snmp_packet->used++;
  } else if(length > 0x7F) {
    if(snmp_packet->used == snmp_packet->max) {
      return 0;
    }

    *snmp_packet->out-- = (uint8_t)length & 0xFF;
    snmp_packet->used++;

    if(snmp_packet->used == snmp_packet->max) {
      return 0;
    }

    *snmp_packet->out-- = 0x81;
    snmp_packet->used++;
  } else {
    if(snmp_packet->used == snmp_packet->max) {
      return 0;
    }

    *snmp_packet->out-- = (uint8_t)length & 0x7F;
    snmp_packet->used++;
  }

  return 1;
}