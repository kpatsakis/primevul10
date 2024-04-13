process_cmd_rtcreport(char *line)
{
  CMD_Request request;
  CMD_Reply reply;
  struct timeval ref_time;
  struct tm ref_time_tm;
  unsigned short n_samples;
  unsigned short n_runs;
  unsigned long span_seconds;
  double coef_seconds_fast;
  double coef_gain_rate_ppm;
  
  request.command = htons(REQ_RTCREPORT);
  if (request_reply(&request, &reply, RPY_RTC, 0)) {
    UTI_TimevalNetworkToHost(&reply.data.rtc.ref_time, &ref_time);
    ref_time_tm = *gmtime(&ref_time.tv_sec);
    n_samples = ntohs(reply.data.rtc.n_samples);
    n_runs = ntohs(reply.data.rtc.n_runs);
    span_seconds = ntohl(reply.data.rtc.span_seconds);
    coef_seconds_fast = UTI_FloatNetworkToHost(reply.data.rtc.rtc_seconds_fast);
    coef_gain_rate_ppm = UTI_FloatNetworkToHost(reply.data.rtc.rtc_gain_rate_ppm);
    printf("RTC ref time (UTC) : %s", asctime(&ref_time_tm));
    printf("Number of samples  : %d\n", n_samples);
    printf("Number of runs     : %d\n", n_runs);
    printf("Sample span period : ");
    print_seconds(span_seconds);
    printf("\n");
    printf("RTC is fast by     : %12.6f seconds\n", coef_seconds_fast);
    printf("RTC gains time at  : %9.3f ppm\n", coef_gain_rate_ppm);
    return 1;
  }
  return 0;
}