NCR_Initialise(void)
{
  do_size_checks();
  do_time_checks();

  logfileid = CNF_GetLogMeasurements() ? LOG_FileOpen("measurements",
      "   Date (UTC) Time     IP Address   L St 123 567 ABCD  LP RP Score    Offset  Peer del. Peer disp.  Root del. Root disp.")
    : -1;

  access_auth_table = ADF_CreateTable();
  broadcasts = ARR_CreateInstance(sizeof (BroadcastDestination));

  /* Server socket will be opened when access is allowed */
  server_sock_fd4 = INVALID_SOCK_FD;
  server_sock_fd6 = INVALID_SOCK_FD;
}