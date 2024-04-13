handle_burst(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  int status;
  IPAddr address, mask;
  UTI_IPNetworkToHost(&rx_message->data.burst.mask, &mask);
  UTI_IPNetworkToHost(&rx_message->data.burst.address, &address);
  status = NSR_InitiateSampleBurst(ntohl(rx_message->data.burst.n_good_samples),
                                   ntohl(rx_message->data.burst.n_total_samples),
                                   &mask, &address);
  
  if (status) {
    tx_message->status = htons(STT_SUCCESS);
  } else {
    tx_message->status = htons(STT_NOSUCHSOURCE);
  }
}