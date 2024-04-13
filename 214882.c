static void parseOptions(int argc, char **argv) {
  int option_idx = 0, do_capture = 0;
  char *__pcap_file = NULL, *bind_mask = NULL;
  int thread_id, opt;
#ifdef linux
  u_int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
#endif

#ifdef DEBUG_TRACE
  trace = fopen("/tmp/ndpiReader.log", "a");

  if(trace) fprintf(trace, " #### %s #### \n", __FUNCTION__);
#endif

#ifdef USE_DPDK
  {
    int ret = rte_eal_init(argc, argv);

    if(ret < 0)
      rte_exit(EXIT_FAILURE, "Error with EAL initialization\n");

    argc -= ret, argv += ret;
  }
#endif

  while((opt = getopt_long(argc, argv, "e:c:C:df:g:i:hp:P:l:s:tv:V:n:Jrp:w:q0123:456:7:89:m:T:U:",
			   longopts, &option_idx)) != EOF) {
#ifdef DEBUG_TRACE
    if(trace) fprintf(trace, " #### -%c [%s] #### \n", opt, optarg ? optarg : "");
#endif

    switch (opt) {
    case 'd':
      enable_protocol_guess = 0;
      break;

    case 'e':
      human_readeable_string_len = atoi(optarg);
      break;

    case 'i':
    case '3':
      _pcap_file[0] = optarg;
      break;

    case 'm':
      pcap_analysis_duration = atol(optarg);
      break;

    case 'f':
    case '6':
      bpfFilter = optarg;
      break;

    case 'g':
      bind_mask = optarg;
      break;

    case 'l':
      num_loops = atoi(optarg);
      break;

    case 'n':
      num_threads = atoi(optarg);
      break;

    case 'p':
      _protoFilePath = optarg;
      break;

    case 'c':
      _customCategoryFilePath = optarg;
      break;

    case 'C':
      if((csv_fp = fopen(optarg, "w")) == NULL)
	printf("Unable to write on CSV file %s\n", optarg);
      break;

    case 's':
      capture_for = atoi(optarg);
      capture_until = capture_for + time(NULL);
      break;

    case 't':
      decode_tunnels = 1;
      break;

    case 'r':
      printf("ndpiReader - nDPI (%s)\n", ndpi_revision());
      exit(0);

    case 'v':
      verbose = atoi(optarg);
      break;

    case 'V':
      nDPI_LogLevel  = atoi(optarg);
      if(nDPI_LogLevel < 0) nDPI_LogLevel = 0;
      if(nDPI_LogLevel > 3) {
	nDPI_LogLevel = 3;
	_debug_protocols = strdup("all");
      }
      break;

    case 'h':
      help(1);
      break;

    case 'J':
      enable_joy_stats = 1;
      break;

    case 'P':
      {
	int _min_pattern_len, _max_pattern_len,
	  _max_num_packets_per_flow, _max_packet_payload_dissection,
	  _max_num_reported_top_payloads;

	enable_payload_analyzer = 1;
	if(sscanf(optarg, "%d:%d:%d:%d:%d", &_min_pattern_len, &_max_pattern_len,
		  &_max_num_packets_per_flow,
		  &_max_packet_payload_dissection,
		  &_max_num_reported_top_payloads) == 5) {
	  min_pattern_len = _min_pattern_len, max_pattern_len = _max_pattern_len;
	  max_num_packets_per_flow = _max_num_packets_per_flow, max_packet_payload_dissection = _max_packet_payload_dissection;
	  max_num_reported_top_payloads = _max_num_reported_top_payloads;
	  if(min_pattern_len > max_pattern_len) min_pattern_len = max_pattern_len;
	  if(min_pattern_len < 2)               min_pattern_len = 2;
	  if(max_pattern_len > 16)              max_pattern_len = 16;
	  if(max_num_packets_per_flow == 0)     max_num_packets_per_flow = 1;
	  if(max_packet_payload_dissection < 4) max_packet_payload_dissection = 4;
	  if(max_num_reported_top_payloads == 0) max_num_reported_top_payloads = 1;
	} else {
	  printf("Invalid -P format. Ignored\n");
	  help(0);
	}
      }
      break;

    case 'w':
      results_path = strdup(optarg);
      if((results_file = fopen(results_path, "w")) == NULL) {
	printf("Unable to write in file %s: quitting\n", results_path);
	return;
      }
      break;

    case 'q':
      quiet_mode = 1;
      nDPI_LogLevel = 0;
      break;

      /* Extcap */
    case '0':
      extcap_interfaces();
      break;

    case '1':
      printf("extcap {version=%s}\n", ndpi_revision());
      break;

    case '2':
      extcap_dlts();
      break;

    case '4':
      extcap_config();
      break;

    case '5':
      do_capture = 1;
      break;

    case '7':
      extcap_capture_fifo = strdup(optarg);
      break;

    case '8':
      nDPI_LogLevel = NDPI_LOG_DEBUG_EXTRA;
      _debug_protocols = strdup("all");
      break;

    case '9':
      extcap_packet_filter = ndpi_get_proto_by_name(ndpi_info_mod, optarg);
      if(extcap_packet_filter == NDPI_PROTOCOL_UNKNOWN) extcap_packet_filter = atoi(optarg);
      break;

    case 257:
      _debug_protocols = strdup(optarg);
      break;

    case 'T':
      max_num_tcp_dissected_pkts = atoi(optarg);
      if(max_num_tcp_dissected_pkts < 3) max_num_tcp_dissected_pkts = 3;
      break;

    case 'U':
      max_num_udp_dissected_pkts = atoi(optarg);
      if(max_num_udp_dissected_pkts < 3) max_num_udp_dissected_pkts = 3;
      break;

    default:
      help(0);
      break;
    }
  }

  if(_pcap_file[0] == NULL)
    help(0);

  if(csv_fp)
    printCSVHeader();

#ifndef USE_DPDK
  if(strchr(_pcap_file[0], ',')) { /* multiple ingress interfaces */
    num_threads = 0;               /* setting number of threads = number of interfaces */
    __pcap_file = strtok(_pcap_file[0], ",");
    while(__pcap_file != NULL && num_threads < MAX_NUM_READER_THREADS) {
      _pcap_file[num_threads++] = __pcap_file;
      __pcap_file = strtok(NULL, ",");
    }
  } else {
    if(num_threads > MAX_NUM_READER_THREADS) num_threads = MAX_NUM_READER_THREADS;
    for(thread_id = 1; thread_id < num_threads; thread_id++)
      _pcap_file[thread_id] = _pcap_file[0];
  }

#ifdef linux
    for(thread_id = 0; thread_id < num_threads; thread_id++)
      core_affinity[thread_id] = -1;

    if(num_cores > 1 && bind_mask != NULL) {
      char *core_id = strtok(bind_mask, ":");
      thread_id = 0;
      while(core_id != NULL && thread_id < num_threads) {
        core_affinity[thread_id++] = atoi(core_id) % num_cores;
        core_id = strtok(NULL, ":");
      }
    }
#endif
#endif

#ifdef DEBUG_TRACE
  if(trace) fclose(trace);
#endif
}