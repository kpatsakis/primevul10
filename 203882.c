NCR_ChangeRemoteAddress(NCR_Instance inst, NTP_Remote_Address *remote_addr)
{
  inst->remote_addr = *remote_addr;
  inst->tx_count = 0;
  inst->presend_done = 0;

  if (inst->mode == MODE_CLIENT)
    close_client_socket(inst);
  else {
    NIO_CloseServerSocket(inst->local_addr.sock_fd);
    inst->local_addr.sock_fd = NIO_OpenServerSocket(remote_addr);
  }

  /* Update the reference ID and reset the source/sourcestats instances */
  SRC_SetRefid(inst->source, UTI_IPToRefid(&remote_addr->ip_addr),
               &inst->remote_addr.ip_addr);
  SRC_ResetInstance(inst->source);
}