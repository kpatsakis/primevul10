static void printFlow(u_int16_t id, struct ndpi_flow_info *flow, u_int16_t thread_id) {
  FILE *out = results_file ? results_file : stdout;
  u_int8_t known_tls;
  char buf[32], buf1[64];
  u_int i;

  double dos_ge_score;
  double dos_slow_score;
  double dos_hulk_score;
  double ddos_score;

  double hearthbleed_score;

  double ftp_patator_score;
  double ssh_patator_score;

  double inf_score;

  if(csv_fp != NULL) {
    float data_ratio = ndpi_data_ratio(flow->src2dst_bytes, flow->dst2src_bytes);
    double f = (double)flow->first_seen, l = (double)flow->last_seen;

    /* PLEASE KEEP IN SYNC WITH printCSVHeader() */
    dos_ge_score = Dos_goldeneye_score(flow);
    
    dos_slow_score = Dos_slow_score(flow);
    dos_hulk_score = Dos_hulk_score(flow);
    ddos_score = Ddos_score(flow);

    hearthbleed_score = Hearthbleed_score(flow);

    ftp_patator_score = Ftp_patator_score(flow);
    ssh_patator_score = Ssh_patator_score(flow);

    inf_score = Infiltration_score(flow);
   
    double benign_score = dos_ge_score < 1 && dos_slow_score < 1 && \
    dos_hulk_score < 1 && ddos_score < 1 && hearthbleed_score < 1 && \
    ftp_patator_score < 1 && ssh_patator_score < 1 && inf_score < 1 ? 1.1 : 0;

    fprintf(csv_fp, "%u,%u,%.3f,%.3f,%.3f,%s,%u,%s,%u,",
	    flow->flow_id,
	    flow->protocol,
	    f/1000.0, l/1000.0,
	    (l-f)/1000.0,
	    flow->src_name, ntohs(flow->src_port),
	    flow->dst_name, ntohs(flow->dst_port)
	    );

    fprintf(csv_fp, "%s,",
	    ndpi_protocol2id(ndpi_thread_info[thread_id].workflow->ndpi_struct,
			     flow->detected_protocol, buf, sizeof(buf)));

    fprintf(csv_fp, "%s,%s,",
	    ndpi_protocol2name(ndpi_thread_info[thread_id].workflow->ndpi_struct,
			       flow->detected_protocol, buf, sizeof(buf)),
	    flow->host_server_name);
    
    fprintf(csv_fp, "%.4lf,%.4lf,%.4lf,%.4lf,%.4lf,%.4lf,%.4lf,%.4lf,%.4lf,", \
	    benign_score, dos_slow_score, dos_ge_score, dos_hulk_score, \
	    ddos_score, hearthbleed_score, ftp_patator_score,		\
	    ssh_patator_score, inf_score);
    
    fprintf(csv_fp, "%u,%llu,%llu,", flow->src2dst_packets,
	    (long long unsigned int) flow->src2dst_bytes, (long long unsigned int) flow->src2dst_goodput_bytes);
    fprintf(csv_fp, "%u,%llu,%llu,", flow->dst2src_packets,
	    (long long unsigned int) flow->dst2src_bytes, (long long unsigned int) flow->dst2src_goodput_bytes);
    fprintf(csv_fp, "%.3f,%s,", data_ratio, ndpi_data_ratio2str(data_ratio));
    fprintf(csv_fp, "%.1f,%.1f,", 100.0*((float)flow->src2dst_goodput_bytes / (float)(flow->src2dst_bytes+1)),
	    100.0*((float)flow->dst2src_goodput_bytes / (float)(flow->dst2src_bytes+1)));
    
    /* IAT (Inter Arrival Time) */
    fprintf(csv_fp, "%u,%.1f,%u,%.1f,",
	    ndpi_data_min(flow->iat_flow), ndpi_data_average(flow->iat_flow), ndpi_data_max(flow->iat_flow), ndpi_data_stddev(flow->iat_flow));

    fprintf(csv_fp, "%u,%.1f,%u,%.1f,%u,%.1f,%u,%.1f,",
	    ndpi_data_min(flow->iat_c_to_s), ndpi_data_average(flow->iat_c_to_s), ndpi_data_max(flow->iat_c_to_s), ndpi_data_stddev(flow->iat_c_to_s),
	    ndpi_data_min(flow->iat_s_to_c), ndpi_data_average(flow->iat_s_to_c), ndpi_data_max(flow->iat_s_to_c), ndpi_data_stddev(flow->iat_s_to_c));

    /* Packet Length */
    fprintf(csv_fp, "%u,%.1f,%u,%.1f,%u,%.1f,%u,%.1f,",
	    ndpi_data_min(flow->pktlen_c_to_s), ndpi_data_average(flow->pktlen_c_to_s), ndpi_data_max(flow->pktlen_c_to_s), ndpi_data_stddev(flow->pktlen_c_to_s),
	    ndpi_data_min(flow->pktlen_s_to_c), ndpi_data_average(flow->pktlen_s_to_c), ndpi_data_max(flow->pktlen_s_to_c), ndpi_data_stddev(flow->pktlen_s_to_c));

    /* TCP flags */
   fprintf(csv_fp, "%d,%d,%d,%d,%d,%d,%d,%d,", flow->cwr_count, flow->ece_count, flow->urg_count, flow->ack_count, flow->psh_count, flow->rst_count, flow->syn_count, flow->fin_count);

   fprintf(csv_fp, "%d,%d,%d,%d,%d,%d,%d,%d,", flow->src2dst_cwr_count, flow->src2dst_ece_count, flow->src2dst_urg_count, flow->src2dst_ack_count, flow->src2dst_psh_count, flow->src2dst_rst_count, flow->src2dst_syn_count, flow->src2dst_fin_count);

   fprintf(csv_fp, "%d,%d,%d,%d,%d,%d,%d,%d,", flow->dst2src_cwr_count, flow->ece_count, flow->urg_count, flow->ack_count, flow->psh_count, flow->rst_count, flow->syn_count, flow->fin_count);

   /* TCP window */
   fprintf(csv_fp, "%u,%u,", flow->c_to_s_init_win, flow->s_to_c_init_win);

    fprintf(csv_fp, "%s,%s,",
	    (flow->ssh_tls.client_requested_server_name[0] != '\0')  ? flow->ssh_tls.client_requested_server_name : "",
	    (flow->ssh_tls.server_info[0] != '\0')  ? flow->ssh_tls.server_info : "");

    fprintf(csv_fp, "%s,%s,%s,%s,%s,",
	    (flow->ssh_tls.ssl_version != 0)        ? ndpi_ssl_version2str(flow->ssh_tls.ssl_version, &known_tls) : "0",
	    (flow->ssh_tls.ja3_client[0] != '\0')   ? flow->ssh_tls.ja3_client : "",
	    (flow->ssh_tls.ja3_client[0] != '\0')   ? is_unsafe_cipher(flow->ssh_tls.client_unsafe_cipher) : "0",
	    (flow->ssh_tls.ja3_server[0] != '\0')   ? flow->ssh_tls.ja3_server : "",
	    (flow->ssh_tls.ja3_server[0] != '\0')   ? is_unsafe_cipher(flow->ssh_tls.server_unsafe_cipher) : "0");
    
    fprintf(csv_fp, "%s,%s,",
	    flow->ssh_tls.tls_alpn                  ? flow->ssh_tls.tls_alpn : "",
	    flow->ssh_tls.tls_supported_versions    ? flow->ssh_tls.tls_supported_versions : "" 
	    );
    fprintf(csv_fp, "%s,%s,",
	    flow->ssh_tls.tls_issuerDN              ? flow->ssh_tls.tls_issuerDN : "",
	    flow->ssh_tls.tls_subjectDN             ? flow->ssh_tls.tls_subjectDN : "" 
	    );
    fprintf(csv_fp, "%s,%s",
	    (flow->ssh_tls.client_hassh[0] != '\0') ? flow->ssh_tls.client_hassh : "",
	    (flow->ssh_tls.server_hassh[0] != '\0') ? flow->ssh_tls.server_hassh : ""
	    );

    fprintf(csv_fp, ",%s", flow->info);
  }

  if((verbose != 1) && (verbose != 2)) {
    if(csv_fp && enable_joy_stats) {
      flowGetBDMeanandVariance(flow);
    }

    if(csv_fp)
      fprintf(csv_fp, "\n");
    return;
  }

  if(csv_fp || (verbose > 1)) {
#if 1
  fprintf(out, "\t%u", id);
#else
  fprintf(out, "\t%u(%u)", id, flow->flow_id);
#endif

  fprintf(out, "\t%s ", ipProto2Name(flow->protocol));

  fprintf(out, "%s%s%s:%u %s %s%s%s:%u ",
	  (flow->ip_version == 6) ? "[" : "",
	  flow->src_name, (flow->ip_version == 6) ? "]" : "", ntohs(flow->src_port),
	  flow->bidirectional ? "<->" : "->",
	  (flow->ip_version == 6) ? "[" : "",
	  flow->dst_name, (flow->ip_version == 6) ? "]" : "", ntohs(flow->dst_port)
	  );

  if(flow->vlan_id > 0) fprintf(out, "[VLAN: %u]", flow->vlan_id);
  if(enable_payload_analyzer) fprintf(out, "[flowId: %u]", flow->flow_id);
  }
  
  if(enable_joy_stats) {
    /* Print entropy values for monitored flows. */
    flowGetBDMeanandVariance(flow);
    fflush(out);
    fprintf(out, "[score: %.4f]", flow->entropy.score);
  }
	
  if(csv_fp) fprintf(csv_fp, "\n");
    
  fprintf(out, "[proto: ");
  if(flow->tunnel_type != ndpi_no_tunnel)
    fprintf(out, "%s:", ndpi_tunnel2str(flow->tunnel_type));

  fprintf(out, "%s/%s]",
	  ndpi_protocol2id(ndpi_thread_info[thread_id].workflow->ndpi_struct,
			   flow->detected_protocol, buf, sizeof(buf)),
	  ndpi_protocol2name(ndpi_thread_info[thread_id].workflow->ndpi_struct,
			     flow->detected_protocol, buf1, sizeof(buf1)));

  if(flow->detected_protocol.category != 0)
    fprintf(out, "[cat: %s/%u]",
	    ndpi_category_get_name(ndpi_thread_info[thread_id].workflow->ndpi_struct,
				   flow->detected_protocol.category),
	    (unsigned int)flow->detected_protocol.category);

  fprintf(out, "[%u pkts/%llu bytes ", flow->src2dst_packets, (long long unsigned int) flow->src2dst_bytes);
  fprintf(out, "%s %u pkts/%llu bytes]",
	  (flow->dst2src_packets > 0) ? "<->" : "->",
	  flow->dst2src_packets, (long long unsigned int) flow->dst2src_bytes);

  fprintf(out, "[Goodput ratio: %.0f/%.0f]",
	  100.0*((float)flow->src2dst_goodput_bytes / (float)(flow->src2dst_bytes+1)),
	  100.0*((float)flow->dst2src_goodput_bytes / (float)(flow->dst2src_bytes+1)));

  if(flow->last_seen > flow->first_seen)
    fprintf(out, "[%.2f sec]", ((float)(flow->last_seen - flow->first_seen))/(float)1000);
  else
    fprintf(out, "[< 1 sec]");

  if(flow->telnet.username[0] != '\0')  fprintf(out, "[Username: %s]", flow->telnet.username);
  if(flow->telnet.password[0] != '\0')  fprintf(out, "[Password: %s]", flow->telnet.password);
  if(flow->host_server_name[0] != '\0') fprintf(out, "[Host: %s]", flow->host_server_name);

  if(flow->info[0] != '\0') fprintf(out, "[%s]", flow->info);
  if(flow->flow_extra_info[0] != '\0') fprintf(out, "[%s]", flow->flow_extra_info);

  if((flow->src2dst_packets+flow->dst2src_packets) > 5) {
    if(flow->iat_c_to_s && flow->iat_s_to_c) {
      float data_ratio = ndpi_data_ratio(flow->src2dst_bytes, flow->dst2src_bytes);

      fprintf(out, "[bytes ratio: %.3f (%s)]", data_ratio, ndpi_data_ratio2str(data_ratio));

      /* IAT (Inter Arrival Time) */
      fprintf(out, "[IAT c2s/s2c min/avg/max/stddev: %u/%u %.0f/%.0f %u/%u %.0f/%.0f]",
	      ndpi_data_min(flow->iat_c_to_s),     ndpi_data_min(flow->iat_s_to_c),
	      (float)ndpi_data_average(flow->iat_c_to_s), (float)ndpi_data_average(flow->iat_s_to_c),
	      ndpi_data_max(flow->iat_c_to_s),     ndpi_data_max(flow->iat_s_to_c),
	      (float)ndpi_data_stddev(flow->iat_c_to_s),  (float)ndpi_data_stddev(flow->iat_s_to_c));

      /* Packet Length */
      fprintf(out, "[Pkt Len c2s/s2c min/avg/max/stddev: %u/%u %.0f/%.0f %u/%u %.0f/%.0f]",
	      ndpi_data_min(flow->pktlen_c_to_s), ndpi_data_min(flow->pktlen_s_to_c),
	      ndpi_data_average(flow->pktlen_c_to_s), ndpi_data_average(flow->pktlen_s_to_c),
	      ndpi_data_max(flow->pktlen_c_to_s), ndpi_data_max(flow->pktlen_s_to_c),
	      ndpi_data_stddev(flow->pktlen_c_to_s),  ndpi_data_stddev(flow->pktlen_s_to_c));
    }
  }

  if(flow->http.url[0] != '\0') {
    ndpi_risk_enum risk = ndpi_validate_url(flow->http.url);

    if(risk != NDPI_NO_RISK)
      NDPI_SET_BIT(flow->risk, risk);
    
    fprintf(out, "[URL: %s][StatusCode: %u]",
	    flow->http.url, flow->http.response_status_code);

    if(flow->http.content_type[0] != '\0')
      fprintf(out, "[Content-Type: %s]", flow->http.content_type);

    if(flow->http.user_agent[0] != '\0')
      fprintf(out, "[User-Agent: %s]", flow->http.user_agent);
  }

  if(flow->risk) {
    u_int i;
    
    fprintf(out, "[Risk: ");

    for(i=0; i<NDPI_MAX_RISK; i++)
      if(NDPI_ISSET_BIT(flow->risk, i))
	fprintf(out, "** %s **", ndpi_risk2str(i));
    
    fprintf(out, "]");
  }
  
  if(flow->ssh_tls.ssl_version != 0) fprintf(out, "[%s]", ndpi_ssl_version2str(flow->ssh_tls.ssl_version, &known_tls));
  if(flow->ssh_tls.client_requested_server_name[0] != '\0') fprintf(out, "[Client: %s]", flow->ssh_tls.client_requested_server_name);
  if(flow->ssh_tls.client_hassh[0] != '\0') fprintf(out, "[HASSH-C: %s]", flow->ssh_tls.client_hassh);

  if(flow->ssh_tls.ja3_client[0] != '\0') fprintf(out, "[JA3C: %s%s]", flow->ssh_tls.ja3_client,
						  print_cipher(flow->ssh_tls.client_unsafe_cipher));

  if(flow->ssh_tls.server_info[0] != '\0') fprintf(out, "[Server: %s]", flow->ssh_tls.server_info);

  if(flow->ssh_tls.server_names) fprintf(out, "[ServerNames: %s]", flow->ssh_tls.server_names);
  if(flow->ssh_tls.server_hassh[0] != '\0') fprintf(out, "[HASSH-S: %s]", flow->ssh_tls.server_hassh);

  if(flow->ssh_tls.ja3_server[0] != '\0') fprintf(out, "[JA3S: %s%s]", flow->ssh_tls.ja3_server,
						  print_cipher(flow->ssh_tls.server_unsafe_cipher));

  if(flow->ssh_tls.tls_issuerDN)  fprintf(out, "[Issuer: %s]", flow->ssh_tls.tls_issuerDN);
  if(flow->ssh_tls.tls_subjectDN) fprintf(out, "[Subject: %s]", flow->ssh_tls.tls_subjectDN);

  if((flow->detected_protocol.master_protocol == NDPI_PROTOCOL_TLS)
     || (flow->detected_protocol.app_protocol == NDPI_PROTOCOL_TLS)) {
    if(flow->ssh_tls.sha1_cert_fingerprint_set) {
      fprintf(out, "[Certificate SHA-1: ");
      for(i=0; i<20; i++)
	fprintf(out, "%s%02X", (i > 0) ? ":" : "",
		flow->ssh_tls.sha1_cert_fingerprint[i] & 0xFF);
      fprintf(out, "]");
    }
  }

  if(flow->ssh_tls.notBefore && flow->ssh_tls.notAfter) {
    char notBefore[32], notAfter[32];
    struct tm a, b;
    struct tm *before = gmtime_r(&flow->ssh_tls.notBefore, &a);
    struct tm *after  = gmtime_r(&flow->ssh_tls.notAfter, &b);

    strftime(notBefore, sizeof(notBefore), "%F %T", before);
    strftime(notAfter, sizeof(notAfter), "%F %T", after);

    fprintf(out, "[Validity: %s - %s]", notBefore, notAfter);
  }

  if(flow->ssh_tls.server_cipher != '\0') fprintf(out, "[Cipher: %s]", ndpi_cipher2str(flow->ssh_tls.server_cipher));
  if(flow->bittorent_hash[0] != '\0') fprintf(out, "[BT Hash: %s]", flow->bittorent_hash);
  if(flow->dhcp_fingerprint[0] != '\0') fprintf(out, "[DHCP Fingerprint: %s]", flow->dhcp_fingerprint);

  if(flow->has_human_readeable_strings) fprintf(out, "[PLAIN TEXT (%s)]", flow->human_readeable_string_buffer);

  fprintf(out, "\n");
}