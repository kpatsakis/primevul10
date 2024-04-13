CAM_Finalise(void)
{
  if (sock_fd4 >= 0) {
    SCH_RemoveInputFileHandler(sock_fd4);
    close(sock_fd4);
  }
  sock_fd4 = -1;
#ifdef HAVE_IPV6
  if (sock_fd6 >= 0) {
    SCH_RemoveInputFileHandler(sock_fd6);
    close(sock_fd6);
  }
  sock_fd6 = -1;
#endif

  ADF_DestroyTable(access_auth_table);

  initialised = 0;
}