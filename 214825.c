static void help(u_int long_help) {
  printf("Welcome to nDPI %s\n\n", ndpi_revision());

  printf("ndpiReader "
#ifndef USE_DPDK
	 "-i <file|device> "
#endif
	 "[-f <filter>][-s <duration>][-m <duration>]\n"
	 "          [-p <protos>][-l <loops> [-q][-d][-J][-h][-e <len>][-t][-v <level>]\n"
	 "          [-n <threads>][-w <file>][-c <file>][-C <file>][-j <file>][-x <file>]\n"
	 "          [-T <num>][-U <num>]\n\n"
	 "Usage:\n"
	 "  -i <file.pcap|device>     | Specify a pcap file/playlist to read packets from or a\n"
	 "                            | device for live capture (comma-separated list)\n"
	 "  -f <BPF filter>           | Specify a BPF filter for filtering selected traffic\n"
	 "  -s <duration>             | Maximum capture duration in seconds (live traffic capture only)\n"
	 "  -m <duration>             | Split analysis duration in <duration> max seconds\n"
	 "  -p <file>.protos          | Specify a protocol file (eg. protos.txt)\n"
	 "  -l <num loops>            | Number of detection loops (test only)\n"
	 "  -n <num threads>          | Number of threads. Default: number of interfaces in -i.\n"
	 "                            | Ignored with pcap files.\n"
#ifdef linux
         "  -g <id:id...>             | Thread affinity mask (one core id per thread)\n"
#endif
	 "  -d                        | Disable protocol guess and use only DPI\n"
	 "  -e <len>                  | Min human readeable string match len. Default %u\n"
	 "  -q                        | Quiet mode\n"
	 "  -J                        | Display flow SPLT (sequence of packet length and time)\n"
	 "                            | and BD (byte distribution). See https://github.com/cisco/joy\n"
	 "  -t                        | Dissect GTP/TZSP tunnels\n"
	 "  -P <a>:<b>:<c>:<d>:<e>    | Enable payload analysis:\n"
	 "                            | <a> = min pattern len to search\n"
	 "                            | <b> = max pattern len to search\n"
	 "                            | <c> = max num packets per flow\n"
	 "                            | <d> = max packet payload dissection\n"
	 "                            | <d> = max num reported payloads\n"
	 "                            | Default: %u:%u:%u:%u:%u\n"
	 "  -r                        | Print nDPI version and git revision\n"
	 "  -c <path>                 | Load custom categories from the specified file\n"
	 "  -C <path>                 | Write output in CSV format on the specified file\n"
	 "  -w <path>                 | Write test output on the specified file. This is useful for\n"
	 "                            | testing purposes in order to compare results across runs\n"
	 "  -h                        | This help\n"
	 "  -v <1|2|3>                | Verbose 'unknown protocol' packet print.\n"
	 "                            | 1 = verbose\n"
	 "                            | 2 = very verbose\n"
	 "                            | 3 = port stats\n"
	 "  -V <1-4>                  | nDPI logging level\n"
	 "                            | 1 - trace, 2 - debug, 3 - full debug\n"
	 "                            | >3 - full debug + dbg_proto = all\n"
	 "  -T <num>                  | Max number of TCP processed packets before giving up [default: %u]\n"
	 "  -U <num>                  | Max number of UDP processed packets before giving up [default: %u]\n"
	 ,
	 human_readeable_string_len,
	 min_pattern_len, max_pattern_len, max_num_packets_per_flow, max_packet_payload_dissection,
	 max_num_reported_top_payloads, max_num_tcp_dissected_pkts, max_num_udp_dissected_pkts);

#ifndef WIN32
  printf("\nExcap (wireshark) options:\n"
	 "  --extcap-interfaces\n"
	 "  --extcap-version\n"
	 "  --extcap-dlts\n"
	 "  --extcap-interface <name>\n"
	 "  --extcap-config\n"
	 "  --capture\n"
	 "  --extcap-capture-filter\n"
	 "  --fifo <path to file or pipe>\n"
	 "  --debug\n"
	 "  --dbg-proto proto|num[,...]\n"
    );
#endif

  if(long_help) {
    NDPI_PROTOCOL_BITMASK all;

    printf("\n\nnDPI supported protocols:\n");
    printf("%3s %-22s %-8s %-12s %s\n", "Id", "Protocol", "Layer_4", "Breed", "Category");
    num_threads = 1;

    NDPI_BITMASK_SET_ALL(all);
    ndpi_set_protocol_detection_bitmask2(ndpi_info_mod, &all);

    ndpi_dump_protocols(ndpi_info_mod);
  }
  exit(!long_help);
}