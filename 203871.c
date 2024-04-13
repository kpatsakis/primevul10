check_packet_format(NTP_Packet *message, int length)
{
  int version;

  /* Check version and length */

  version = NTP_LVM_TO_VERSION(message->lvm);
  if (version < NTP_MIN_COMPAT_VERSION || version > NTP_MAX_COMPAT_VERSION) {
    DEBUG_LOG(LOGF_NtpCore, "NTP packet has invalid version %d", version);
    return 0;
  } 

  if (length < NTP_NORMAL_PACKET_LENGTH || (unsigned int)length % 4) {
    DEBUG_LOG(LOGF_NtpCore, "NTP packet has invalid length %d", length);
    return 0;
  }

  /* We can't reliably check the packet for invalid extension fields as we
     support MACs longer than the shortest valid extension field */

  return 1;
}