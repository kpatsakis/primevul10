NCR_Finalise(void)
{
  unsigned int i;

  if (server_sock_fd4 != INVALID_SOCK_FD)
    NIO_CloseServerSocket(server_sock_fd4);
  if (server_sock_fd6 != INVALID_SOCK_FD)
    NIO_CloseServerSocket(server_sock_fd6);

  for (i = 0; i < ARR_GetSize(broadcasts); i++)
    NIO_CloseServerSocket(((BroadcastDestination *)ARR_GetElement(broadcasts, i))->local_addr.sock_fd);

  ARR_DestroyInstance(broadcasts);
  ADF_DestroyTable(access_auth_table);
}