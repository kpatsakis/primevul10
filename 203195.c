handle_modify_maxupdateskew(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  REF_ModifyMaxupdateskew(UTI_FloatNetworkToHost(rx_message->data.modify_maxupdateskew.new_max_update_skew));
  tx_message->status = htons(STT_SUCCESS);
}