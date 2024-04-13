process_cmd_waitsync(char *line)
{
  CMD_Request request;
  CMD_Reply reply;
  uint32_t ref_id, a, b, c, d;
  double correction, skew_ppm, max_correction, max_skew_ppm;
  int ret = 0, max_tries, i;

  max_tries = 0;
  max_correction = 0.0;
  max_skew_ppm = 0.0;

  sscanf(line, "%d %lf %lf", &max_tries, &max_correction, &max_skew_ppm);

  request.command = htons(REQ_TRACKING);

  for (i = 1; ; i++) {
    if (request_reply(&request, &reply, RPY_TRACKING, 0)) {
      ref_id = ntohl(reply.data.tracking.ref_id);
      a = (ref_id >> 24);
      b = (ref_id >> 16) & 0xff;
      c = (ref_id >> 8) & 0xff;
      d = (ref_id) & 0xff;

      correction = UTI_FloatNetworkToHost(reply.data.tracking.current_correction);
      correction = fabs(correction);
      skew_ppm = UTI_FloatNetworkToHost(reply.data.tracking.skew_ppm);

      printf("try: %d, refid: %d.%d.%d.%d, correction: %.9f, skew: %.3f\n",
          i, a, b, c, d, correction, skew_ppm);

      if (ref_id != 0 && ref_id != 0x7f7f0101L /* LOCAL refid */ &&
          (max_correction == 0.0 || correction <= max_correction) &&
          (max_skew_ppm == 0.0 || skew_ppm <= max_skew_ppm)) {
        ret = 1;
      }
    }

    if (!ret && (!max_tries || i < max_tries)) {
      sleep(10);
    } else {
      break;
    }
  }
  return ret;
}