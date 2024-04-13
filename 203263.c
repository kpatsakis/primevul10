process_cmd_tracking(char *line)
{
  CMD_Request request;
  CMD_Reply reply;
  IPAddr ip_addr;
  uint32_t ref_id;
  char host[50];
  char *ref_ip;
  struct timeval ref_time;
  struct tm ref_time_tm;
  unsigned long a, b, c, d;
  double correction;
  double last_offset;
  double rms_offset;
  double freq_ppm;
  double resid_freq_ppm;
  double skew_ppm;
  double root_delay;
  double root_dispersion;
  double last_update_interval;
  const char *leap_status;
  
  request.command = htons(REQ_TRACKING);
  if (request_reply(&request, &reply, RPY_TRACKING, 0)) {
    ref_id = ntohl(reply.data.tracking.ref_id);
    a = (ref_id >> 24);
    b = (ref_id >> 16) & 0xff;
    c = (ref_id >> 8) & 0xff;
    d = (ref_id) & 0xff;
    
    UTI_IPNetworkToHost(&reply.data.tracking.ip_addr, &ip_addr);
    if (ip_addr.family == IPADDR_UNSPEC) {
      ref_ip = UTI_RefidToString(ref_id);
    } else if (no_dns) {
      ref_ip = UTI_IPToString(&ip_addr);
    } else {
      DNS_IPAddress2Name(&ip_addr, host, sizeof (host));
      ref_ip = host;
    }
    
    switch (ntohs(reply.data.tracking.leap_status)) {
      case LEAP_Normal:
        leap_status = "Normal";
        break;
      case LEAP_InsertSecond:
        leap_status = "Insert second";
        break;
      case LEAP_DeleteSecond:
        leap_status = "Delete second";
        break;
      case LEAP_Unsynchronised:
        leap_status = "Not synchronised";
        break;
      default:
        leap_status = "Unknown";
        break;
    }

    printf("Reference ID    : %lu.%lu.%lu.%lu (%s)\n", a, b, c, d, ref_ip);
    printf("Stratum         : %lu\n", (unsigned long) ntohs(reply.data.tracking.stratum));
    UTI_TimevalNetworkToHost(&reply.data.tracking.ref_time, &ref_time);
    ref_time_tm = *gmtime((time_t *)&ref_time.tv_sec);
    printf("Ref time (UTC)  : %s", asctime(&ref_time_tm));
    correction = UTI_FloatNetworkToHost(reply.data.tracking.current_correction);
    last_offset = UTI_FloatNetworkToHost(reply.data.tracking.last_offset);
    rms_offset = UTI_FloatNetworkToHost(reply.data.tracking.rms_offset);
    printf("System time     : %.9f seconds %s of NTP time\n", fabs(correction),
           (correction > 0.0) ? "slow" : "fast");
    printf("Last offset     : %.9f seconds\n", last_offset);
    printf("RMS offset      : %.9f seconds\n", rms_offset);
    freq_ppm = UTI_FloatNetworkToHost(reply.data.tracking.freq_ppm);
    resid_freq_ppm = UTI_FloatNetworkToHost(reply.data.tracking.resid_freq_ppm);
    skew_ppm = UTI_FloatNetworkToHost(reply.data.tracking.skew_ppm);
    root_delay = UTI_FloatNetworkToHost(reply.data.tracking.root_delay);
    root_dispersion = UTI_FloatNetworkToHost(reply.data.tracking.root_dispersion);
    last_update_interval = UTI_FloatNetworkToHost(reply.data.tracking.last_update_interval);
    printf("Frequency       : %.3f ppm %s\n", fabs(freq_ppm), (freq_ppm < 0.0) ? "slow" : "fast"); 
    printf("Residual freq   : %.3f ppm\n", resid_freq_ppm);
    printf("Skew            : %.3f ppm\n", skew_ppm);
    printf("Root delay      : %.6f seconds\n", root_delay);
    printf("Root dispersion : %.6f seconds\n", root_dispersion);
    printf("Update interval : %.1f seconds\n", last_update_interval);
    printf("Leap status     : %s\n", leap_status);
    return 1;
  }
  return 0;
}