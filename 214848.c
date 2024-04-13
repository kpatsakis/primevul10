static void printFlowsStats() {
  int thread_id;
  u_int32_t total_flows = 0;
  FILE *out = results_file ? results_file : stdout;

  if(enable_payload_analyzer)
    ndpi_report_payload_stats();

  for(thread_id = 0; thread_id < num_threads; thread_id++)
    total_flows += ndpi_thread_info[thread_id].workflow->num_allocated_flows;

  if((all_flows = (struct flow_info*)malloc(sizeof(struct flow_info)*total_flows)) == NULL) {
    fprintf(out, "Fatal error: not enough memory\n");
    exit(-1);
  }

  if(verbose) {
    ndpi_host_ja3_fingerprints *ja3ByHostsHashT = NULL; // outer hash table
    ndpi_ja3_fingerprints_host *hostByJA3C_ht = NULL;   // for client
    ndpi_ja3_fingerprints_host *hostByJA3S_ht = NULL;   // for server
    int i;
    ndpi_host_ja3_fingerprints *ja3ByHost_element = NULL;
    ndpi_ja3_info *info_of_element = NULL;
    ndpi_host_ja3_fingerprints *tmp = NULL;
    ndpi_ja3_info *tmp2 = NULL;
    unsigned int num_ja3_client;
    unsigned int num_ja3_server;
    
    fprintf(out, "\n");

    num_flows = 0;
    for(thread_id = 0; thread_id < num_threads; thread_id++) {
      for(i=0; i<NUM_ROOTS; i++)
	ndpi_twalk(ndpi_thread_info[thread_id].workflow->ndpi_flows_root[i],
		   node_print_known_proto_walker, &thread_id);
    }

    if((verbose == 2) || (verbose == 3)) {
      for(i = 0; i < num_flows; i++) {
	ndpi_host_ja3_fingerprints *ja3ByHostFound = NULL;
	ndpi_ja3_fingerprints_host *hostByJA3Found = NULL;

	//check if this is a ssh-ssl flow
	if(all_flows[i].flow->ssh_tls.ja3_client[0] != '\0'){
	  //looking if the host is already in the hash table
	  HASH_FIND_INT(ja3ByHostsHashT, &(all_flows[i].flow->src_ip), ja3ByHostFound);

	  //host ip -> ja3
	  if(ja3ByHostFound == NULL){
	    //adding the new host
	    ndpi_host_ja3_fingerprints *newHost = malloc(sizeof(ndpi_host_ja3_fingerprints));
	    newHost->host_client_info_hasht = NULL;
	    newHost->host_server_info_hasht = NULL;
	    newHost->ip_string = all_flows[i].flow->src_name;
	    newHost->ip = all_flows[i].flow->src_ip;
	    newHost->dns_name = all_flows[i].flow->ssh_tls.client_requested_server_name;

	    ndpi_ja3_info *newJA3 = malloc(sizeof(ndpi_ja3_info));
	    newJA3->ja3 = all_flows[i].flow->ssh_tls.ja3_client;
	    newJA3->unsafe_cipher = all_flows[i].flow->ssh_tls.client_unsafe_cipher;
	    //adding the new ja3 fingerprint
	    HASH_ADD_KEYPTR(hh, newHost->host_client_info_hasht,
			    newJA3->ja3, strlen(newJA3->ja3), newJA3);
	    //adding the new host
	    HASH_ADD_INT(ja3ByHostsHashT, ip, newHost);
	  } else {
	    //host already in the hash table
	    ndpi_ja3_info *infoFound = NULL;

	    HASH_FIND_STR(ja3ByHostFound->host_client_info_hasht,
			  all_flows[i].flow->ssh_tls.ja3_client, infoFound);

	    if(infoFound == NULL){
	      ndpi_ja3_info *newJA3 = malloc(sizeof(ndpi_ja3_info));
	      newJA3->ja3 = all_flows[i].flow->ssh_tls.ja3_client;
	      newJA3->unsafe_cipher = all_flows[i].flow->ssh_tls.client_unsafe_cipher;
	      HASH_ADD_KEYPTR(hh, ja3ByHostFound->host_client_info_hasht,
			      newJA3->ja3, strlen(newJA3->ja3), newJA3);
	    }
	  }

	  //ja3 -> host ip
	  HASH_FIND_STR(hostByJA3C_ht, all_flows[i].flow->ssh_tls.ja3_client, hostByJA3Found);
	  if(hostByJA3Found == NULL){
	    ndpi_ip_dns *newHost = malloc(sizeof(ndpi_ip_dns));

	    newHost->ip = all_flows[i].flow->src_ip;
	    newHost->ip_string = all_flows[i].flow->src_name;
	    newHost->dns_name = all_flows[i].flow->ssh_tls.client_requested_server_name;;

	    ndpi_ja3_fingerprints_host *newElement = malloc(sizeof(ndpi_ja3_fingerprints_host));
	    newElement->ja3 = all_flows[i].flow->ssh_tls.ja3_client;
	    newElement->unsafe_cipher = all_flows[i].flow->ssh_tls.client_unsafe_cipher;
	    newElement->ipToDNS_ht = NULL;

	    HASH_ADD_INT(newElement->ipToDNS_ht, ip, newHost);
	    HASH_ADD_KEYPTR(hh, hostByJA3C_ht, newElement->ja3, strlen(newElement->ja3),
			    newElement);
	  } else {
	    ndpi_ip_dns *innerElement = NULL;
	    HASH_FIND_INT(hostByJA3Found->ipToDNS_ht, &(all_flows[i].flow->src_ip), innerElement);
	    if(innerElement == NULL){
	      ndpi_ip_dns *newInnerElement = malloc(sizeof(ndpi_ip_dns));
	      newInnerElement->ip = all_flows[i].flow->src_ip;
	      newInnerElement->ip_string = all_flows[i].flow->src_name;
	      newInnerElement->dns_name = all_flows[i].flow->ssh_tls.client_requested_server_name;
	      HASH_ADD_INT(hostByJA3Found->ipToDNS_ht, ip, newInnerElement);
	    }
	  }
	}

	if(all_flows[i].flow->ssh_tls.ja3_server[0] != '\0'){
	  //looking if the host is already in the hash table
	  HASH_FIND_INT(ja3ByHostsHashT, &(all_flows[i].flow->dst_ip), ja3ByHostFound);
	  if(ja3ByHostFound == NULL){
	    //adding the new host in the hash table
	    ndpi_host_ja3_fingerprints *newHost = malloc(sizeof(ndpi_host_ja3_fingerprints));
	    newHost->host_client_info_hasht = NULL;
	    newHost->host_server_info_hasht = NULL;
	    newHost->ip_string = all_flows[i].flow->dst_name;
	    newHost->ip = all_flows[i].flow->dst_ip;
	    newHost->dns_name = all_flows[i].flow->ssh_tls.server_info;

	    ndpi_ja3_info *newJA3 = malloc(sizeof(ndpi_ja3_info));
	    newJA3->ja3 = all_flows[i].flow->ssh_tls.ja3_server;
	    newJA3->unsafe_cipher = all_flows[i].flow->ssh_tls.server_unsafe_cipher;
	    //adding the new ja3 fingerprint
	    HASH_ADD_KEYPTR(hh, newHost->host_server_info_hasht, newJA3->ja3,
			    strlen(newJA3->ja3), newJA3);
	    //adding the new host
	    HASH_ADD_INT(ja3ByHostsHashT, ip, newHost);
	  } else {
	    //host already in the hashtable
	    ndpi_ja3_info *infoFound = NULL;
	    HASH_FIND_STR(ja3ByHostFound->host_server_info_hasht,
			  all_flows[i].flow->ssh_tls.ja3_server, infoFound);
	    if(infoFound == NULL){
	      ndpi_ja3_info *newJA3 = malloc(sizeof(ndpi_ja3_info));
	      newJA3->ja3 = all_flows[i].flow->ssh_tls.ja3_server;
	      newJA3->unsafe_cipher = all_flows[i].flow->ssh_tls.server_unsafe_cipher;
	      HASH_ADD_KEYPTR(hh, ja3ByHostFound->host_server_info_hasht,
			      newJA3->ja3, strlen(newJA3->ja3), newJA3);
	    }
	  }

	  HASH_FIND_STR(hostByJA3S_ht, all_flows[i].flow->ssh_tls.ja3_server, hostByJA3Found);
	  if(hostByJA3Found == NULL){
	    ndpi_ip_dns *newHost = malloc(sizeof(ndpi_ip_dns));

	    newHost->ip = all_flows[i].flow->dst_ip;
	    newHost->ip_string = all_flows[i].flow->dst_name;
	    newHost->dns_name = all_flows[i].flow->ssh_tls.server_info;;

	    ndpi_ja3_fingerprints_host *newElement = malloc(sizeof(ndpi_ja3_fingerprints_host));
	    newElement->ja3 = all_flows[i].flow->ssh_tls.ja3_server;
	    newElement->unsafe_cipher = all_flows[i].flow->ssh_tls.server_unsafe_cipher;
	    newElement->ipToDNS_ht = NULL;

	    HASH_ADD_INT(newElement->ipToDNS_ht, ip, newHost);
	    HASH_ADD_KEYPTR(hh, hostByJA3S_ht, newElement->ja3, strlen(newElement->ja3),
			    newElement);
	  } else {
	    ndpi_ip_dns *innerElement = NULL;

	    HASH_FIND_INT(hostByJA3Found->ipToDNS_ht, &(all_flows[i].flow->dst_ip), innerElement);
	    if(innerElement == NULL){
	      ndpi_ip_dns *newInnerElement = malloc(sizeof(ndpi_ip_dns));
	      newInnerElement->ip = all_flows[i].flow->dst_ip;
	      newInnerElement->ip_string = all_flows[i].flow->dst_name;
	      newInnerElement->dns_name = all_flows[i].flow->ssh_tls.server_info;
	      HASH_ADD_INT(hostByJA3Found->ipToDNS_ht, ip, newInnerElement);
	    }
	  }

	}
      }

      if(ja3ByHostsHashT) {
	ndpi_ja3_fingerprints_host *hostByJA3Element = NULL;
	ndpi_ja3_fingerprints_host *tmp3 = NULL;
	ndpi_ip_dns *innerHashEl = NULL;
	ndpi_ip_dns *tmp4 = NULL;

	if(verbose == 2) {
	  /* for each host the number of flow with a ja3 fingerprint is printed */
	  i = 1;

	  fprintf(out, "JA3 Host Stats: \n");
	  fprintf(out, "\t\t IP %-24s \t %-10s \n", "Address", "# JA3C");

	  for(ja3ByHost_element = ja3ByHostsHashT; ja3ByHost_element != NULL;
	      ja3ByHost_element = ja3ByHost_element->hh.next) {
	    num_ja3_client = HASH_COUNT(ja3ByHost_element->host_client_info_hasht);
	    num_ja3_server = HASH_COUNT(ja3ByHost_element->host_server_info_hasht);

	    if(num_ja3_client > 0) {
	      fprintf(out, "\t%d\t %-24s \t %-7u\n",
		      i,
		      ja3ByHost_element->ip_string,
		      num_ja3_client
		      );
	      i++;
	    }

	  }
	} else if(verbose == 3) {
	  int i = 1;
	  int againstRepeat;
	  ndpi_ja3_fingerprints_host *hostByJA3Element = NULL;
	  ndpi_ja3_fingerprints_host *tmp3 = NULL;
	  ndpi_ip_dns *innerHashEl = NULL;
	  ndpi_ip_dns *tmp4 = NULL;

	  //for each host it is printted the JA3C and JA3S, along the server name (if any)
	  //and the security status

	  fprintf(out, "JA3C/JA3S Host Stats: \n");
	  fprintf(out, "\t%-7s %-24s %-34s %s\n", "", "IP", "JA3C", "JA3S");

	  //reminder
	  //ja3ByHostsHashT: hash table <ip, (ja3, ht_client, ht_server)>
	  //ja3ByHost_element: element of ja3ByHostsHashT
	  //info_of_element: element of the inner hash table of ja3ByHost_element
	  HASH_ITER(hh, ja3ByHostsHashT, ja3ByHost_element, tmp) {
	    num_ja3_client = HASH_COUNT(ja3ByHost_element->host_client_info_hasht);
	    num_ja3_server = HASH_COUNT(ja3ByHost_element->host_server_info_hasht);
	    againstRepeat = 0;
	    if(num_ja3_client > 0) {
	      HASH_ITER(hh, ja3ByHost_element->host_client_info_hasht, info_of_element, tmp2) {
		fprintf(out, "\t%-7d %-24s %s %s\n",
			i,
			ja3ByHost_element->ip_string,
			info_of_element->ja3,
			print_cipher(info_of_element->unsafe_cipher)
			);
		againstRepeat = 1;
		i++;
	      }
	    }

	    if(num_ja3_server > 0) {
	      HASH_ITER(hh, ja3ByHost_element->host_server_info_hasht, info_of_element, tmp2) {
		fprintf(out, "\t%-7d %-24s %-34s %s %s %s%s%s\n",
			i,
			ja3ByHost_element->ip_string,
			"",
			info_of_element->ja3,
			print_cipher(info_of_element->unsafe_cipher),
			ja3ByHost_element->dns_name[0] ? "[" : "",
			ja3ByHost_element->dns_name,
			ja3ByHost_element->dns_name[0] ? "]" : ""
			);
		i++;
	      }
	    }
	  }

	  i = 1;

	  fprintf(out, "\nIP/JA3 Distribution:\n");
	  fprintf(out, "%-15s %-39s %-26s\n", "", "JA3", "IP");
	  HASH_ITER(hh, hostByJA3C_ht, hostByJA3Element, tmp3) {
	    againstRepeat = 0;
	    HASH_ITER(hh, hostByJA3Element->ipToDNS_ht, innerHashEl, tmp4) {
	      if(againstRepeat == 0) {
		fprintf(out, "\t%-7d JA3C %s",
			i,
			hostByJA3Element->ja3
			);
		fprintf(out, "   %-15s %s\n",
			innerHashEl->ip_string,
			print_cipher(hostByJA3Element->unsafe_cipher)
			);
		againstRepeat = 1;
		i++;
	      } else {
		fprintf(out, "\t%45s", "");
		fprintf(out, "   %-15s %s\n",
			innerHashEl->ip_string,
			print_cipher(hostByJA3Element->unsafe_cipher)
			);
	      }
	    }
	  }
	  HASH_ITER(hh, hostByJA3S_ht, hostByJA3Element, tmp3) {
	    againstRepeat = 0;
	    HASH_ITER(hh, hostByJA3Element->ipToDNS_ht, innerHashEl, tmp4) {
	      if(againstRepeat == 0) {
		fprintf(out, "\t%-7d JA3S %s",
			i,
			hostByJA3Element->ja3
			);
		fprintf(out, "   %-15s %-10s %s%s%s\n",
			innerHashEl->ip_string,
			print_cipher(hostByJA3Element->unsafe_cipher),
			innerHashEl->dns_name[0] ? "[" : "",
			innerHashEl->dns_name,
			innerHashEl->dns_name[0] ? "]" : ""
			);
		againstRepeat = 1;
		i++;
	      } else {
		fprintf(out, "\t%45s", "");
		fprintf(out, "   %-15s %-10s %s%s%s\n",
			innerHashEl->ip_string,
			print_cipher(hostByJA3Element->unsafe_cipher),
			innerHashEl->dns_name[0] ? "[" : "",
			innerHashEl->dns_name,
			innerHashEl->dns_name[0] ? "]" : ""
			);
	      }
	    }
	  }
	}
	fprintf(out, "\n\n");

	//freeing the hash table
	HASH_ITER(hh, ja3ByHostsHashT, ja3ByHost_element, tmp) {
	  HASH_ITER(hh, ja3ByHost_element->host_client_info_hasht, info_of_element, tmp2) {
	    if(ja3ByHost_element->host_client_info_hasht)
	      HASH_DEL(ja3ByHost_element->host_client_info_hasht, info_of_element);
	    free(info_of_element);
	  }
	  HASH_ITER(hh, ja3ByHost_element->host_server_info_hasht, info_of_element, tmp2) {
	    if(ja3ByHost_element->host_server_info_hasht)
	      HASH_DEL(ja3ByHost_element->host_server_info_hasht, info_of_element);
	    free(info_of_element);
	  }
	  HASH_DEL(ja3ByHostsHashT, ja3ByHost_element);
	  free(ja3ByHost_element);
	}

	HASH_ITER(hh, hostByJA3C_ht, hostByJA3Element, tmp3) {
	  HASH_ITER(hh, hostByJA3C_ht->ipToDNS_ht, innerHashEl, tmp4) {
	    if(hostByJA3Element->ipToDNS_ht)
	      HASH_DEL(hostByJA3Element->ipToDNS_ht, innerHashEl);
	    free(innerHashEl);
	  }
	  HASH_DEL(hostByJA3C_ht, hostByJA3Element);
	  free(hostByJA3Element);
	}

	hostByJA3Element = NULL;
	HASH_ITER(hh, hostByJA3S_ht, hostByJA3Element, tmp3) {
	  HASH_ITER(hh, hostByJA3S_ht->ipToDNS_ht, innerHashEl, tmp4) {
	    if(hostByJA3Element->ipToDNS_ht)
	      HASH_DEL(hostByJA3Element->ipToDNS_ht, innerHashEl);
	    free(innerHashEl);
	  }
	  HASH_DEL(hostByJA3S_ht, hostByJA3Element);
	  free(hostByJA3Element);
	}
      }
    }

    /* Print all flows stats */

    qsort(all_flows, num_flows, sizeof(struct flow_info), cmpFlows);

    if(verbose > 1) {
      for(i=0; i<num_flows; i++)
	printFlow(i+1, all_flows[i].flow, all_flows[i].thread_id);
    }

    for(thread_id = 0; thread_id < num_threads; thread_id++) {
      if(ndpi_thread_info[thread_id].workflow->stats.protocol_counter[0 /* 0 = Unknown */] > 0) {
	fprintf(out, "\n\nUndetected flows:%s\n",
		undetected_flows_deleted ? " (expired flows are not listed below)" : "");
	break;
      }
    }

    num_flows = 0;
    for(thread_id = 0; thread_id < num_threads; thread_id++) {
      if(ndpi_thread_info[thread_id].workflow->stats.protocol_counter[0] > 0) {
	for(i=0; i<NUM_ROOTS; i++)
	  ndpi_twalk(ndpi_thread_info[thread_id].workflow->ndpi_flows_root[i],
		     node_print_unknown_proto_walker, &thread_id);
      }
    }

    qsort(all_flows, num_flows, sizeof(struct flow_info), cmpFlows);

    for(i=0; i<num_flows; i++)
      printFlow(i+1, all_flows[i].flow, all_flows[i].thread_id);

  } else if(csv_fp != NULL) {
    int i;

    num_flows = 0;
    for(thread_id = 0; thread_id < num_threads; thread_id++) {
      for(i=0; i<NUM_ROOTS; i++)
	ndpi_twalk(ndpi_thread_info[thread_id].workflow->ndpi_flows_root[i],
		   node_print_known_proto_walker, &thread_id);
    }

    for(i=0; i<num_flows; i++)
	printFlow(i+1, all_flows[i].flow, all_flows[i].thread_id);
  }

  free(all_flows);
}