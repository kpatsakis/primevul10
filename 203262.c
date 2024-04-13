process_cmd_sources(char *line)
{
  CMD_Request request;
  CMD_Reply reply;
  int n_sources, i;
  int verbose = 0;

  double orig_latest_meas, latest_meas, latest_meas_err;
  IPAddr ip_addr;
  uint32_t latest_meas_ago;
  uint16_t poll, stratum;
  uint16_t state, mode, flags, reachability;
  char hostname_buf[50];

  /* Check whether to output verbose headers */
  verbose = check_for_verbose_flag(line);
  
  request.command = htons(REQ_N_SOURCES);
  if (request_reply(&request, &reply, RPY_N_SOURCES, 0)) {
    n_sources = ntohl(reply.data.n_sources.n_sources);
    printf("210 Number of sources = %d\n", n_sources);
    if (verbose) {
      printf("\n");
      printf("  .-- Source mode  '^' = server, '=' = peer, '#' = local clock.\n");
      printf(" / .- Source state '*' = current synced, '+' = combined , '-' = not combined,\n");
      printf("| /   '?' = unreachable, 'x' = time may be in error, '~' = time too variable.\n");
      printf("||                                                 .- xxxx [ yyyy ] +/- zzzz\n");
      printf("||                                                /   xxxx = adjusted offset,\n");
      printf("||         Log2(Polling interval) -.             |    yyyy = measured offset,\n");
      printf("||                                  \\            |    zzzz = estimated error.\n");
      printf("||                                   |           |                         \n");
    }

    printf("MS Name/IP address         Stratum Poll Reach LastRx Last sample\n");
    printf("===============================================================================\n");

    /*     "MS NNNNNNNNNNNNNNNNNNNNNNNNNNN  SS  PP   RRR  RRRR  SSSSSSS[SSSSSSS] +/- SSSSSS" */

    for (i=0; i<n_sources; i++) {
      request.command = htons(REQ_SOURCE_DATA);
      request.data.source_data.index = htonl(i);
      if (request_reply(&request, &reply, RPY_SOURCE_DATA, 0)) {
          UTI_IPNetworkToHost(&reply.data.source_data.ip_addr, &ip_addr);
          poll = ntohs(reply.data.source_data.poll);
          stratum = ntohs(reply.data.source_data.stratum);
          state = ntohs(reply.data.source_data.state);
          mode = ntohs(reply.data.source_data.mode);
          flags = ntohs(reply.data.source_data.flags);
          reachability = ntohs(reply.data.source_data.reachability);
          latest_meas_ago = ntohl(reply.data.source_data.since_sample);
          orig_latest_meas = UTI_FloatNetworkToHost(reply.data.source_data.orig_latest_meas);
          latest_meas = UTI_FloatNetworkToHost(reply.data.source_data.latest_meas);
          latest_meas_err = UTI_FloatNetworkToHost(reply.data.source_data.latest_meas_err);

          if (mode == RPY_SD_MD_REF) {
            snprintf(hostname_buf, sizeof(hostname_buf), "%s", UTI_RefidToString(ip_addr.addr.in4));
          } else if (no_dns) {
            snprintf(hostname_buf, sizeof(hostname_buf), "%s", UTI_IPToString(&ip_addr));
          } else {
            DNS_IPAddress2Name(&ip_addr, hostname_buf, sizeof(hostname_buf));
            hostname_buf[25] = 0;
          }

          switch (mode) {
            case RPY_SD_MD_CLIENT:
              printf("^"); break;
            case RPY_SD_MD_PEER:
              printf("="); break;
            case RPY_SD_MD_REF:
              printf("#"); break;
            default:
              printf(" ");
          }
          switch (state) {
            case RPY_SD_ST_SYNC:
              printf("*"); break;
            case RPY_SD_ST_UNREACH:
              printf("?"); break;
            case RPY_SD_ST_FALSETICKER:
              printf("x"); break;
            case RPY_SD_ST_JITTERY:
              printf("~"); break;
            case RPY_SD_ST_CANDIDATE:
              printf("+"); break;
            case RPY_SD_ST_OUTLIER:
              printf("-"); break;
            default:
              printf(" ");
          }
          switch (flags) {
            default:
              break;
          }

          printf(" %-27s  %2d  %2d   %3o  ", hostname_buf, stratum, poll, reachability);
          print_seconds(latest_meas_ago);
          printf("  ");
          print_signed_nanoseconds(latest_meas);
          printf("[");
          print_signed_nanoseconds(orig_latest_meas);
          printf("]");
          printf(" +/- ");
          print_nanoseconds(latest_meas_err);
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