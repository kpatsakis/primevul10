void printCSVHeader() {
  if(!csv_fp) return;

  fprintf(csv_fp, "#flow_id,protocol,first_seen,last_seen,duration,src_ip,src_port,dst_ip,dst_port,ndpi_proto_num,ndpi_proto,server_name,");
  fprintf(csv_fp, "benign_score,dos_slow_score,dos_goldeneye_score,dos_hulk_score,ddos_score,hearthbleed_score,ftp_patator_score,ssh_patator_score,infiltration_score,");
  fprintf(csv_fp, "c_to_s_pkts,c_to_s_bytes,c_to_s_goodput_bytes,s_to_c_pkts,s_to_c_bytes,s_to_c_goodput_bytes,");
  fprintf(csv_fp, "data_ratio,str_data_ratio,c_to_s_goodput_ratio,s_to_c_goodput_ratio,");

  /* IAT (Inter Arrival Time) */
  fprintf(csv_fp, "iat_flow_min,iat_flow_avg,iat_flow_max,iat_flow_stddev,");
  fprintf(csv_fp, "iat_c_to_s_min,iat_c_to_s_avg,iat_c_to_s_max,iat_c_to_s_stddev,");
  fprintf(csv_fp, "iat_s_to_c_min,iat_s_to_c_avg,iat_s_to_c_max,iat_s_to_c_stddev,");

  /* Packet Length */
  fprintf(csv_fp, "pktlen_c_to_s_min,pktlen_c_to_s_avg,pktlen_c_to_s_max,pktlen_c_to_s_stddev,");
  fprintf(csv_fp, "pktlen_s_to_c_min,pktlen_s_to_c_avg,pktlen_s_to_c_max,pktlen_s_to_c_stddev,");

  /* TCP flags */
  fprintf(csv_fp, "cwr,ece,urg,ack,psh,rst,syn,fin,");

  fprintf(csv_fp, "c_to_s_cwr,c_to_s_ece,c_to_s_urg,c_to_s_ack,c_to_s_psh,c_to_s_rst,c_to_s_syn,c_to_s_fin,");

  fprintf(csv_fp, "s_to_c_cwr,s_to_c_ece,s_to_c_urg,s_to_c_ack,s_to_c_psh,s_to_c_rst,s_to_c_syn,s_to_c_fin,");

  /* TCP window */
  fprintf(csv_fp, "c_to_s_init_win,s_to_c_init_win,");

  /* Flow info */
  fprintf(csv_fp, "client_info,server_info,");
  fprintf(csv_fp, "tls_version,ja3c,tls_client_unsafe,");
  fprintf(csv_fp, "ja3s,tls_server_unsafe,");
  fprintf(csv_fp, "tls_alpn,tls_supported_versions,");
  fprintf(csv_fp, "tls_issuerDN,tls_subjectDN,");  
  fprintf(csv_fp, "ssh_client_hassh,ssh_server_hassh,flow_info");
  
  /* Joy */
  if(enable_joy_stats) {
    fprintf(csv_fp, ",byte_dist_mean,byte_dist_std,entropy,total_entropy");  
  }
  
  fprintf(csv_fp, "\n");
}