process_cmd_manual_list(const char *line)
{
  CMD_Request request;
  CMD_Reply reply;
  int n_samples;
  RPY_ManualListSample *sample;
  int i;
  struct timeval when;
  double slewed_offset, orig_offset, residual;

  request.command = htons(REQ_MANUAL_LIST);
  if (request_reply(&request, &reply, RPY_MANUAL_LIST, 0)) {
          n_samples = ntohl(reply.data.manual_list.n_samples);
          printf("210 n_samples = %d\n", n_samples);
          printf("#    Date  Time(UTC)    Slewed   Original   Residual\n"
                 "====================================================\n");
          for (i=0; i<n_samples; i++) {
            sample = &reply.data.manual_list.samples[i];
            UTI_TimevalNetworkToHost(&sample->when, &when);
            slewed_offset = UTI_FloatNetworkToHost(sample->slewed_offset);
            orig_offset = UTI_FloatNetworkToHost(sample->orig_offset);
            residual = UTI_FloatNetworkToHost(sample->residual);
            printf("%2d %s %10.2f %10.2f %10.2f\n", i, time_to_log_form(when.tv_sec), slewed_offset, orig_offset, residual);
          }
          return 1;
  }
  return 0;
}