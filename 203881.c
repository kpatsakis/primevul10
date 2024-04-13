transmit_packet(NTP_Mode my_mode, /* The mode this machine wants to be */
                int my_poll, /* The log2 of the local poll interval */
                int version, /* The NTP version to be set in the packet */
                int do_auth, /* Boolean indicating whether to authenticate the packet or not */
                uint32_t key_id, /* The authentication key ID */
                NTP_int64 *orig_ts, /* Originate timestamp (from received packet) */
                struct timeval *local_rx, /* Local time request packet was received */
                struct timeval *local_tx, /* RESULT : Time this reply
                                             is sent as local time, or
                                             NULL if don't want to
                                             know */
                NTP_int64 *local_ntp_tx, /* RESULT : Time reply sent
                                            as NTP timestamp
                                            (including adjustment to
                                            reference), ignored if
                                            NULL */
                NTP_Remote_Address *where_to, /* Where to address the reponse to */
                NTP_Local_Address *from /* From what address to send it */
                )
{
  NTP_Packet message;
  int leap, auth_len, length, ret;
  struct timeval local_receive, local_transmit;

  /* Parameters read from reference module */
  int are_we_synchronised, our_stratum, smooth_time;
  NTP_Leap leap_status;
  uint32_t our_ref_id, ts_fuzz;
  struct timeval our_ref_time;
  double our_root_delay, our_root_dispersion, smooth_offset;

  /* Don't reply with version higher than ours */
  if (version > NTP_VERSION) {
    version = NTP_VERSION;
  }

  /* This is accurate enough and cheaper than calling LCL_ReadCookedTime.
     A more accurate time stamp will be taken later in this function. */
  SCH_GetLastEventTime(&local_transmit, NULL, NULL);

  REF_GetReferenceParams(&local_transmit,
                         &are_we_synchronised, &leap_status,
                         &our_stratum,
                         &our_ref_id, &our_ref_time,
                         &our_root_delay, &our_root_dispersion);

  /* Get current smoothing offset when sending packet to a client */
  if (SMT_IsEnabled() && (my_mode == MODE_SERVER || my_mode == MODE_BROADCAST)) {
    smooth_offset = SMT_GetOffset(&local_transmit);
    smooth_time = fabs(smooth_offset) > LCL_GetSysPrecisionAsQuantum();

    /* Suppress leap second when smoothing and slew mode are enabled */
    if (REF_GetLeapMode() == REF_LeapModeSlew &&
        (leap_status == LEAP_InsertSecond || leap_status == LEAP_DeleteSecond))
      leap_status = LEAP_Normal;
  } else {
    smooth_time = 0;
    smooth_offset = 0.0;
  }

  if (smooth_time) {
    our_ref_id = NTP_REFID_SMOOTH;
    UTI_AddDoubleToTimeval(&our_ref_time, smooth_offset, &our_ref_time);
    UTI_AddDoubleToTimeval(local_rx, smooth_offset, &local_receive);
  } else {
    local_receive = *local_rx;
  }

  if (are_we_synchronised) {
    leap = (int) leap_status;
  } else {
    leap = LEAP_Unsynchronised;
  }

  /* Generate transmit packet */
  message.lvm = NTP_LVM(leap, version, my_mode);
  /* Stratum 16 and larger are invalid */
  if (our_stratum < NTP_MAX_STRATUM) {
    message.stratum = our_stratum;
  } else {
    message.stratum = NTP_INVALID_STRATUM;
  }
 
  message.poll = my_poll;
  message.precision = LCL_GetSysPrecisionAsLog();

  /* If we're sending a client mode packet and we aren't synchronized yet, 
     we might have to set up artificial values for some of these parameters */
  message.root_delay = UTI_DoubleToInt32(our_root_delay);
  message.root_dispersion = UTI_DoubleToInt32(our_root_dispersion);

  message.reference_id = htonl(our_ref_id);

  /* Now fill in timestamps */

  UTI_TimevalToInt64(&our_ref_time, &message.reference_ts, 0);

  /* Originate - this comes from the last packet the source sent us */
  message.originate_ts = *orig_ts;

  /* Receive - this is when we received the last packet from the source.
     This timestamp will have been adjusted so that it will now look to
     the source like we have been running on our latest estimate of
     frequency all along */
  UTI_TimevalToInt64(&local_receive, &message.receive_ts, 0);

  /* Prepare random bits which will be added to the transmit timestamp. */
  ts_fuzz = UTI_GetNTPTsFuzz(message.precision);

  /* Transmit - this our local time right now!  Also, we might need to
     store this for our own use later, next time we receive a message
     from the source we're sending to now. */
  LCL_ReadCookedTime(&local_transmit, NULL);

  if (smooth_time)
    UTI_AddDoubleToTimeval(&local_transmit, smooth_offset, &local_transmit);

  length = NTP_NORMAL_PACKET_LENGTH;

  /* Authenticate */
  if (do_auth && key_id) {
    /* Pre-compensate the transmit time by approx. how long it will
       take to generate the authentication data. */
    local_transmit.tv_usec += KEY_GetAuthDelay(key_id);
    UTI_NormaliseTimeval(&local_transmit);
    UTI_TimevalToInt64(&local_transmit, &message.transmit_ts, ts_fuzz);

    auth_len = KEY_GenerateAuth(key_id, (unsigned char *) &message,
        offsetof(NTP_Packet, auth_keyid),
        (unsigned char *)&message.auth_data, sizeof (message.auth_data));
    if (auth_len > 0) {
      message.auth_keyid = htonl(key_id);
      length += sizeof (message.auth_keyid) + auth_len;
    } else {
      DEBUG_LOG(LOGF_NtpCore,
                "Could not generate auth data with key %"PRIu32" to send packet",
                key_id);
      return 0;
    }
  } else {
    if (do_auth) {
      /* Zero key ID means crypto-NAK, append only the ID without any data */
      message.auth_keyid = 0;
      length += sizeof (message.auth_keyid);
    }
    UTI_TimevalToInt64(&local_transmit, &message.transmit_ts, ts_fuzz);
  }

  ret = NIO_SendPacket(&message, where_to, from, length);

  if (local_tx) {
    *local_tx = local_transmit;
  }

  if (local_ntp_tx) {
    *local_ntp_tx = message.transmit_ts;
  }

  return ret;
}