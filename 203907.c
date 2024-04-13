check_packet_auth(NTP_Packet *pkt, int length, int *has_auth, uint32_t *key_id)
{
  int i, remainder, ext_length;
  unsigned char *data;
  uint32_t id;

  /* Go through extension fields and see if there is a valid MAC */

  i = NTP_NORMAL_PACKET_LENGTH;
  data = (void *)pkt;

  while (1) {
    remainder = length - i;

    /* Check if the remaining data is a valid MAC.  This needs to be done
       before trying to parse it as an extension field, because we support
       MACs longer than the shortest valid extension field. */
    if (remainder >= NTP_MIN_MAC_LENGTH && remainder <= NTP_MAX_MAC_LENGTH) {
      id = ntohl(*(uint32_t *)(data + i));
      if (KEY_CheckAuth(id, (void *)pkt, i, (void *)(data + i + 4),
                        remainder - 4)) {
        if (key_id)
          *key_id = id;
        if (has_auth)
          *has_auth = 1;
        return 1;
      }
    }

    /* Check if this is a valid field extension.  They consist of 16-bit type,
       16-bit length of the whole field aligned to 32 bits and data. */
    if (remainder >= NTP_MIN_EXTENSION_LENGTH) {
      ext_length = ntohs(*(uint16_t *)(data + i + 2));
      if (ext_length >= NTP_MIN_EXTENSION_LENGTH &&
          ext_length <= remainder && ext_length % 4 == 0) {
        i += ext_length;
        continue;
      }
    }

    /* Invalid or missing MAC, or format error */
    break;
  }

  /* This is not 100% reliable as a MAC could fail to authenticate and could
     pass as an extension field, leaving reminder smaller than the minimum MAC
     length.  Not a big problem, at worst we won't reply with a crypto-NAK. */
  if (has_auth)
    *has_auth = remainder >= NTP_MIN_MAC_LENGTH;

  return 0;
}