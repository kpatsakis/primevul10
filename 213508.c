static void processCertificateElements(struct ndpi_detection_module_struct *ndpi_struct,
				       struct ndpi_flow_struct *flow,
				       u_int16_t p_offset, u_int16_t certificate_len) {
  struct ndpi_packet_struct *packet = &flow->packet;
  u_int num_found = 0, i;
  char buffer[64] = { '\0' }, rdnSeqBuf[2048] = { '\0' };
  u_int rdn_len = 0;

#ifdef DEBUG_TLS
  printf("[TLS] %s() [offset: %u][certificate_len: %u]\n", __FUNCTION__, p_offset, certificate_len);
#endif

  /* Check after handshake protocol header (5 bytes) and message header (4 bytes) */
  for(i = p_offset; i < certificate_len; i++) {
    /*
       See https://www.ibm.com/support/knowledgecenter/SSFKSJ_7.5.0/com.ibm.mq.sec.doc/q009860_.htm
       for X.509 certificate labels
    */
    if((packet->payload[i] == 0x55) && (packet->payload[i+1] == 0x04) && (packet->payload[i+2] == 0x03)) {
      /* Common Name */
      int rc = extractRDNSequence(packet, i, buffer, sizeof(buffer), rdnSeqBuf, &rdn_len, sizeof(rdnSeqBuf), "CN");
      if(rc == -1) break;

#ifdef DEBUG_TLS
      printf("[TLS] %s() [%s][%s: %s]\n", __FUNCTION__, (num_found == 0) ? "Subject" : "Issuer", "Common Name", buffer);
#endif
    } else if((packet->payload[i] == 0x55) && (packet->payload[i+1] == 0x04) && (packet->payload[i+2] == 0x06)) {
      /* Country */
      int rc = extractRDNSequence(packet, i, buffer, sizeof(buffer), rdnSeqBuf, &rdn_len, sizeof(rdnSeqBuf), "C");
      if(rc == -1) break;

#ifdef DEBUG_TLS
      printf("[TLS] %s() [%s][%s: %s]\n", __FUNCTION__, (num_found == 0) ? "Subject" : "Issuer", "Country", buffer);
#endif
    } else if((packet->payload[i] == 0x55) && (packet->payload[i+1] == 0x04) && (packet->payload[i+2] == 0x07)) {
      /* Locality */
      int rc = extractRDNSequence(packet, i, buffer, sizeof(buffer), rdnSeqBuf, &rdn_len, sizeof(rdnSeqBuf), "L");
      if(rc == -1) break;

#ifdef DEBUG_TLS
      printf("[TLS] %s() [%s][%s: %s]\n", __FUNCTION__, (num_found == 0) ? "Subject" : "Issuer", "Locality", buffer);
#endif
    } else if((packet->payload[i] == 0x55) && (packet->payload[i+1] == 0x04) && (packet->payload[i+2] == 0x08)) {
      /* State or Province */
      int rc = extractRDNSequence(packet, i, buffer, sizeof(buffer), rdnSeqBuf, &rdn_len, sizeof(rdnSeqBuf), "ST");
      if(rc == -1) break;

#ifdef DEBUG_TLS
      printf("[TLS] %s() [%s][%s: %s]\n", __FUNCTION__, (num_found == 0) ? "Subject" : "Issuer", "State or Province", buffer);
#endif
    } else if((packet->payload[i] == 0x55) && (packet->payload[i+1] == 0x04) && (packet->payload[i+2] == 0x0a)) {
      /* Organization Name */
      int rc = extractRDNSequence(packet, i, buffer, sizeof(buffer), rdnSeqBuf, &rdn_len, sizeof(rdnSeqBuf), "O");
      if(rc == -1) break;

#ifdef DEBUG_TLS
      printf("[TLS] %s() [%s][%s: %s]\n", __FUNCTION__, (num_found == 0) ? "Subject" : "Issuer", "Organization Name", buffer);
#endif

    } else if((packet->payload[i] == 0x55) && (packet->payload[i+1] == 0x04) && (packet->payload[i+2] == 0x0b)) {
      /* Organization Unit */
      int rc = extractRDNSequence(packet, i, buffer, sizeof(buffer), rdnSeqBuf, &rdn_len, sizeof(rdnSeqBuf), "OU");
      if(rc == -1) break;

#ifdef DEBUG_TLS
      printf("[TLS] %s() [%s][%s: %s]\n", __FUNCTION__, (num_found == 0) ? "Subject" : "Issuer", "Organization Unit", buffer);
#endif
    } else if((packet->payload[i] == 0x30) && (packet->payload[i+1] == 0x1e) && (packet->payload[i+2] == 0x17)) {
      /* Certificate Validity */
      u_int8_t len = packet->payload[i+3];
      u_int offset = i+4;

      if(num_found == 0) {
	num_found++;

#ifdef DEBUG_TLS
	printf("[TLS] %s() IssuerDN [%s]\n", __FUNCTION__, rdnSeqBuf);
#endif

	if(rdn_len && (flow->protos.tls_quic_stun.tls_quic.issuerDN == NULL))
	  flow->protos.tls_quic_stun.tls_quic.issuerDN = ndpi_strdup(rdnSeqBuf);

	rdn_len = 0; /* Reset buffer */
      }

      if((offset+len) < packet->payload_packet_len) {
	char utcDate[32];

#ifdef DEBUG_TLS
	u_int j;

	printf("[CERTIFICATE] notBefore [len: %u][", len);
	for(j=0; j<len; j++) printf("%c", packet->payload[i+4+j]);
	printf("]\n");
#endif

	if(len < (sizeof(utcDate)-1)) {
	  struct tm utc;
	  utc.tm_isdst = -1; /* Not set by strptime */

	  strncpy(utcDate, (const char*)&packet->payload[i+4], len);
	  utcDate[len] = '\0';

	  /* 141021000000Z */
	  if(strptime(utcDate, "%y%m%d%H%M%SZ", &utc) != NULL) {
	    flow->protos.tls_quic_stun.tls_quic.notBefore = timegm(&utc);
#ifdef DEBUG_TLS
	    printf("[CERTIFICATE] notBefore %u [%s]\n",
		   flow->protos.tls_quic_stun.tls_quic.notBefore, utcDate);
#endif
	  }
	}

	offset += len;

	if((offset+1) < packet->payload_packet_len) {
	  len = packet->payload[offset+1];

	  offset += 2;

	  if((offset+len) < packet->payload_packet_len) {
	    u_int32_t time_sec = flow->packet.current_time_ms / 1000;
#ifdef DEBUG_TLS
	    u_int j;

	    printf("[CERTIFICATE] notAfter [len: %u][", len);
	    for(j=0; j<len; j++) printf("%c", packet->payload[offset+j]);
	    printf("]\n");
#endif

	    if(len < (sizeof(utcDate)-1)) {
	      struct tm utc;
	      utc.tm_isdst = -1; /* Not set by strptime */

	      strncpy(utcDate, (const char*)&packet->payload[offset], len);
	      utcDate[len] = '\0';

	      /* 141021000000Z */
	      if(strptime(utcDate, "%y%m%d%H%M%SZ", &utc) != NULL) {
		flow->protos.tls_quic_stun.tls_quic.notAfter = timegm(&utc);
#ifdef DEBUG_TLS
		printf("[CERTIFICATE] notAfter %u [%s]\n",
		       flow->protos.tls_quic_stun.tls_quic.notAfter, utcDate);
#endif
	      }
	    }


	    if((time_sec < flow->protos.tls_quic_stun.tls_quic.notBefore)
	       || (time_sec > flow->protos.tls_quic_stun.tls_quic.notAfter))
	    ndpi_set_risk(flow, NDPI_TLS_CERTIFICATE_EXPIRED); /* Certificate expired */
	  }
	}
      }
    } else if((packet->payload[i] == 0x55) && (packet->payload[i+1] == 0x1d) && (packet->payload[i+2] == 0x11)) {
      /* Organization OID: 2.5.29.17 (subjectAltName) */
      u_int8_t matched_name = 0;

#ifdef DEBUG_TLS
      printf("******* [TLS] Found subjectAltName\n");
#endif

      i += 3 /* skip the initial patten 55 1D 11 */;
      i++; /* skip the first type, 0x04 == BIT STRING, and jump to it's length */
      if(i < packet->payload_packet_len) {
	i += (packet->payload[i] & 0x80) ? (packet->payload[i] & 0x7F) : 0; /* skip BIT STRING length */
	if(i < packet->payload_packet_len) {
	  i += 2; /* skip the second type, 0x30 == SEQUENCE, and jump to it's length */
	  if(i < packet->payload_packet_len) {
	    i += (packet->payload[i] & 0x80) ? (packet->payload[i] & 0x7F) : 0; /* skip SEQUENCE length */
	    i++;

	    while(i < packet->payload_packet_len) {
	      if(packet->payload[i] == 0x82) {
		if((i < (packet->payload_packet_len - 1))
		   && ((i + packet->payload[i + 1] + 2) < packet->payload_packet_len)) {
		  u_int8_t len = packet->payload[i + 1];
		  char dNSName[256];

		  i += 2;

		  /* The check "len > sizeof(dNSName) - 1" will be always false. If we add it,
		     the compiler is smart enough to detect it and throws a warning */
		  if((len == 0 /* Looks something went wrong */)
		     || ((i+len) >  packet->payload_packet_len))
		    break;

		  strncpy(dNSName, (const char*)&packet->payload[i], len);
		  dNSName[len] = '\0';

		  cleanupServerName(dNSName, len);

#if DEBUG_TLS
		  printf("[TLS] dNSName %s [%s][len: %u][leftover: %d]\n", dNSName,
			 flow->protos.tls_quic_stun.tls_quic.client_requested_server_name, len,
			 packet->payload_packet_len-i-len);
#endif
		  if(matched_name == 0) {
		    if(flow->protos.tls_quic_stun.tls_quic.client_requested_server_name[0] == '\0')
		      matched_name = 1;	/* No SNI */
		    else if (dNSName[0] == '*')
		    {
		      char * label = strstr(flow->protos.tls_quic_stun.tls_quic.client_requested_server_name, &dNSName[1]);

		      if (label != NULL)
		      {
		        char * first_dot = strchr(flow->protos.tls_quic_stun.tls_quic.client_requested_server_name, '.');

		        if (first_dot == NULL || first_dot >= label)
		        {
		          matched_name = 1;
		        }
		      }
		    }
		    else if(strcmp(flow->protos.tls_quic_stun.tls_quic.client_requested_server_name, dNSName) == 0)
		      matched_name = 1;
		  }

		  if(flow->protos.tls_quic_stun.tls_quic.server_names == NULL)
		    flow->protos.tls_quic_stun.tls_quic.server_names = ndpi_strdup(dNSName),
		      flow->protos.tls_quic_stun.tls_quic.server_names_len = strlen(dNSName);
		  else {
		    u_int16_t dNSName_len = strlen(dNSName);
		    u_int16_t newstr_len = flow->protos.tls_quic_stun.tls_quic.server_names_len + dNSName_len + 1;
		    char *newstr = (char*)ndpi_realloc(flow->protos.tls_quic_stun.tls_quic.server_names,
						       flow->protos.tls_quic_stun.tls_quic.server_names_len+1, newstr_len+1);

		    if(newstr) {
		      flow->protos.tls_quic_stun.tls_quic.server_names = newstr;
		      flow->protos.tls_quic_stun.tls_quic.server_names[flow->protos.tls_quic_stun.tls_quic.server_names_len] = ',';
		      strncpy(&flow->protos.tls_quic_stun.tls_quic.server_names[flow->protos.tls_quic_stun.tls_quic.server_names_len+1],
			      dNSName, dNSName_len+1);
		      flow->protos.tls_quic_stun.tls_quic.server_names[newstr_len] = '\0';
		      flow->protos.tls_quic_stun.tls_quic.server_names_len = newstr_len;
		    }
		  }

		  if(!flow->l4.tcp.tls.subprotocol_detected)
		    if(ndpi_match_hostname_protocol(ndpi_struct, flow, NDPI_PROTOCOL_TLS, dNSName, len))
		      flow->l4.tcp.tls.subprotocol_detected = 1;

		  i += len;
		} else {
#if DEBUG_TLS
		  printf("[TLS] Leftover %u bytes", packet->payload_packet_len - i);
#endif
		  break;
		}
	      } else {
		break;
	      }
	    } /* while */

	    if(!matched_name)
	      ndpi_set_risk(flow, NDPI_TLS_CERTIFICATE_MISMATCH); /* Certificate mismatch */
	  }
	}
      }
    }
  }

  if(rdn_len && (flow->protos.tls_quic_stun.tls_quic.subjectDN == NULL)) {
    flow->protos.tls_quic_stun.tls_quic.subjectDN = ndpi_strdup(rdnSeqBuf);

    if(flow->detected_protocol_stack[1] == NDPI_PROTOCOL_UNKNOWN) {
      /* No idea what is happening behind the scenes: let's check the certificate */
      u_int32_t proto_id;
      int rc = ndpi_match_string_value(ndpi_struct->tls_cert_subject_automa.ac_automa,
				       rdnSeqBuf, strlen(rdnSeqBuf),&proto_id);

      if(rc == 0) {
	/* Match found */
	ndpi_protocol ret = { NDPI_PROTOCOL_TLS, proto_id, NDPI_PROTOCOL_CATEGORY_UNSPECIFIED};

	flow->detected_protocol_stack[0] = proto_id,
	  flow->detected_protocol_stack[1] = NDPI_PROTOCOL_TLS;

	flow->category = ndpi_get_proto_category(ndpi_struct, ret);
	ndpi_check_subprotocol_risk(flow, proto_id);

	if(ndpi_struct->tls_cert_cache == NULL)
	  ndpi_struct->tls_cert_cache = ndpi_lru_cache_init(1024);

	if(ndpi_struct->tls_cert_cache && flow->packet.iph) {
	  u_int32_t key = flow->packet.iph->daddr + flow->packet.tcp->dest;

	  ndpi_lru_add_to_cache(ndpi_struct->tls_cert_cache, key, proto_id);
	}
      }
    }
  }

  if(flow->protos.tls_quic_stun.tls_quic.subjectDN && flow->protos.tls_quic_stun.tls_quic.issuerDN
     && (!strcmp(flow->protos.tls_quic_stun.tls_quic.subjectDN, flow->protos.tls_quic_stun.tls_quic.issuerDN)))
    ndpi_set_risk(flow, NDPI_TLS_SELFSIGNED_CERTIFICATE);

#if DEBUG_TLS
  printf("[TLS] %s() SubjectDN [%s]\n", __FUNCTION__, rdnSeqBuf);
#endif
}