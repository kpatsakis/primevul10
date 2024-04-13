close_client_socket(NCR_Instance inst)
{
  if (inst->mode == MODE_CLIENT && inst->local_addr.sock_fd != INVALID_SOCK_FD) {
    NIO_CloseClientSocket(inst->local_addr.sock_fd);
    inst->local_addr.sock_fd = INVALID_SOCK_FD;
  }
}