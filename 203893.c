NCR_DestroyInstance(NCR_Instance instance)
{
  if (instance->opmode != MD_OFFLINE)
    take_offline(instance);

  if (instance->mode == MODE_ACTIVE)
    NIO_CloseServerSocket(instance->local_addr.sock_fd);

  /* This will destroy the source instance inside the
     structure, which will cause reselection if this was the
     synchronising source etc. */
  SRC_DestroyInstance(instance->source);

  /* Free the data structure */
  Free(instance);
}