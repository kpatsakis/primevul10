NCR_AddBroadcastDestination(IPAddr *addr, unsigned short port, int interval)
{
  BroadcastDestination *destination;

  destination = (BroadcastDestination *)ARR_GetNewElement(broadcasts);

  destination->addr.ip_addr = *addr;
  destination->addr.port = port;
  destination->local_addr.ip_addr.family = IPADDR_UNSPEC;
  destination->local_addr.sock_fd = NIO_OpenServerSocket(&destination->addr);
  destination->interval = interval;

  SCH_AddTimeoutInClass(destination->interval, SAMPLING_SEPARATION, SAMPLING_RANDOMNESS,
                        SCH_NtpBroadcastClass, broadcast_timeout,
                        (void *)(long)(ARR_GetSize(broadcasts) - 1));
}