handle_writertc(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  switch (RTC_WriteParameters()) {
    case RTC_ST_OK:
      tx_message->status = htons(STT_SUCCESS);
      break;
    case RTC_ST_NODRV:
      tx_message->status = htons(STT_NORTC);
      break;
    case RTC_ST_BADFILE:
      tx_message->status = htons(STT_BADRTCFILE);
      break;
  }
}