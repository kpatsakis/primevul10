process_cmd_sourcestats(char *line)
{
  CMD_Request request;
  CMD_Reply reply;
  int n_sources, i;
  int verbose = 0;

  char hostname_buf[50];
  unsigned long n_samples, n_runs, span_seconds;
  double resid_freq_ppm, skew_ppm, sd, est_offset;
  uint32_t ref_id;
  IPAddr ip_addr;

  verbose = check_for_verbose_flag(line);

  request.command = htons(REQ_N_SOURCES);
  if (request_reply(&request, &reply, RPY_N_SOURCES, 0)) {
    n_sources = ntohl(reply.data.n_sources.n_sources);
    printf("210 Number of sources = %d\n", n_sources);
    if (verbose) {
      printf("                             .- Number of sample points in measurement set.\n");
      printf("                            /    .- Number of residual runs with same sign.\n");
      printf("                           |    /    .- Length of measurement set (time).\n");
      printf("                           |   |    /      .- Est. clock freq error (ppm).\n");
      printf("                           |   |   |      /           .- Est. error in freq.\n");
      printf("                           |   |   |     |           /         .- Est. offset.\n");
      printf("                           |   |   |     |          |          |   On the -.\n");
      printf("                           |   |   |     |          |          |   samples. \\\n");
      printf("                           |   |   |     |          |          |             |\n");
    }

    printf("Name/IP Address            NP  NR  Span  Frequency  Freq Skew  Offset  Std Dev\n");
    printf("==============================================================================\n");

    /*      NNNNNNNNNNNNNNNNNNNNNNNNN  NP  NR  SSSS FFFFFFFFFF SSSSSSSSSS  SSSSSSS  SSSSSS*/

    for (i=0; i<n_sources; i++) {
      request.command = htons(REQ_SOURCESTATS);
      request.data.source_data.index = htonl(i);
      if (request_reply(&request, &reply, RPY_SOURCESTATS, 0)) {
          ref_id = ntohl(reply.data.sourcestats.ref_id);
          UTI_IPNetworkToHost(&reply.data.sourcestats.ip_addr, &ip_addr);
          n_samples = ntohl(reply.data.sourcestats.n_samples);
          n_runs = ntohl(reply.data.sourcestats.n_runs);
          span_seconds = ntohl(reply.data.sourcestats.span_seconds);
          resid_freq_ppm = UTI_FloatNetworkToHost(reply.data.sourcestats.resid_freq_ppm);
          skew_ppm = UTI_FloatNetworkToHost(reply.data.sourcestats.skew_ppm);
          sd = UTI_FloatNetworkToHost(reply.data.sourcestats.sd);
          est_offset = UTI_FloatNetworkToHost(reply.data.sourcestats.est_offset);
          /* est_offset_err = UTI_FloatNetworkToHost(reply.data.sourcestats.est_offset_err); */

          if (ip_addr.family == IPADDR_UNSPEC)
            snprintf(hostname_buf, sizeof(hostname_buf), "%s", UTI_RefidToString(ref_id));
          else if (no_dns) {
            snprintf(hostname_buf, sizeof(hostname_buf), "%s", UTI_IPToString(&ip_addr));
          } else {
            DNS_IPAddress2Name(&ip_addr, hostname_buf, sizeof(hostname_buf));
            hostname_buf[25] = 0;
          }

          printf("%-25s %3lu %3lu  ", hostname_buf, n_samples, n_runs);
          print_seconds(span_seconds);
          printf(" ");
          print_freq_ppm(resid_freq_ppm);
          printf(" ");
          print_freq_ppm(skew_ppm);
          printf("  ");
          print_signed_nanoseconds(est_offset);
          printf("  ");
          print_nanoseconds(sd);
          printf("\n");
      } else {
        return 0;
      }
    }
  } else {
    return 0;
  }
  return 1;
}