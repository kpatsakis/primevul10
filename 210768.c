int processClientServerHello(struct ndpi_detection_module_struct *ndpi_struct,
			     struct ndpi_flow_struct *flow, uint32_t quic_version) {
  struct ndpi_packet_struct *packet = &flow->packet;
  union ja3_info ja3;
  u_int8_t invalid_ja3 = 0;
  u_int16_t tls_version, ja3_str_len;
  char ja3_str[JA3_STR_LEN];
  ndpi_MD5_CTX ctx;
  u_char md5_hash[16];
  int i;
  u_int16_t total_len;
  u_int8_t handshake_type;
  char buffer[64] = { '\0' };
  int is_quic = (quic_version != 0);
  int is_dtls = packet->udp && (!is_quic);

#ifdef DEBUG_TLS
  printf("TLS %s() called\n", __FUNCTION__);
#endif

  memset(&ja3, 0, sizeof(ja3));

  handshake_type = packet->payload[0];
  total_len = (packet->payload[1] << 16) +  (packet->payload[2] << 8) + packet->payload[3];

  if((total_len > packet->payload_packet_len) || (packet->payload[1] != 0x0))
    return(0); /* Not found */

  total_len = packet->payload_packet_len;

  /* At least "magic" 3 bytes, null for string end, otherwise no need to waste cpu cycles */
  if(total_len > 4) {
    u_int16_t base_offset    = (!is_dtls) ? 38 : 46;
    u_int16_t version_offset = (!is_dtls) ? 4 : 12;
    u_int16_t offset = (!is_dtls) ? 38 : 46, extension_len, j;
    u_int8_t  session_id_len =  0;

    if((base_offset >= total_len) ||
       (version_offset + 1) >= total_len)
      return 0; /* Not found */

    session_id_len = packet->payload[base_offset];

#ifdef DEBUG_TLS
    printf("TLS [len: %u][handshake_type: %02X]\n", packet->payload_packet_len, handshake_type);
#endif

    tls_version = ntohs(*((u_int16_t*)&packet->payload[version_offset]));

    if(handshake_type == 0x02 /* Server Hello */) {
      int i, rc;

      ja3.server.tls_handshake_version = tls_version;

#ifdef DEBUG_TLS
      printf("TLS Server Hello [version: 0x%04X]\n", tls_version);
#endif

      /*
	The server hello decides about the TLS version of this flow
	https://networkengineering.stackexchange.com/questions/55752/why-does-wireshark-show-version-tls-1-2-here-instead-of-tls-1-3
      */
      if(packet->udp)
	offset += session_id_len + 1;
      else {
	if(tls_version < 0x7F15 /* TLS 1.3 lacks of session id */)
	  offset += session_id_len+1;
      }

      if((offset+3) > packet->payload_packet_len)
	return(0); /* Not found */

      ja3.server.num_cipher = 1, ja3.server.cipher[0] = ntohs(*((u_int16_t*)&packet->payload[offset]));
      if((flow->protos.tls_quic_stun.tls_quic.server_unsafe_cipher = ndpi_is_safe_ssl_cipher(ja3.server.cipher[0])) == 1)
	ndpi_set_risk(flow, NDPI_TLS_WEAK_CIPHER);

      flow->protos.tls_quic_stun.tls_quic.server_cipher = ja3.server.cipher[0];

#ifdef DEBUG_TLS
      printf("TLS [server][session_id_len: %u][cipher: %04X]\n", session_id_len, ja3.server.cipher[0]);
#endif

      offset += 2 + 1;

      if((offset + 1) < packet->payload_packet_len) /* +1 because we are goint to read 2 bytes */
	extension_len = ntohs(*((u_int16_t*)&packet->payload[offset]));
      else
	extension_len = 0;

#ifdef DEBUG_TLS
      printf("TLS [server][extension_len: %u]\n", extension_len);
#endif
      offset += 2;

      for(i=0; i<extension_len; ) {
	u_int16_t extension_id, extension_len;

	if((offset+4) > packet->payload_packet_len) break;

	extension_id  = ntohs(*((u_int16_t*)&packet->payload[offset]));
	extension_len = ntohs(*((u_int16_t*)&packet->payload[offset+2]));

	if(ja3.server.num_tls_extension < MAX_NUM_JA3)
	  ja3.server.tls_extension[ja3.server.num_tls_extension++] = extension_id;

#ifdef DEBUG_TLS
	printf("TLS [server][extension_id: %u/0x%04X][len: %u]\n",
	       extension_id, extension_id, extension_len);
#endif

	if(extension_id == 43 /* supported versions */) {
	  if(extension_len >= 2) {
	    u_int16_t tls_version = ntohs(*((u_int16_t*)&packet->payload[offset+4]));

#ifdef DEBUG_TLS
	    printf("TLS [server] [TLS version: 0x%04X]\n", tls_version);
#endif

	    flow->protos.tls_quic_stun.tls_quic.ssl_version = ja3.server.tls_supported_version = tls_version;
	  }
	} else if(extension_id == 16 /* application_layer_protocol_negotiation (ALPN) */) {
	  u_int16_t s_offset = offset+4;
	  u_int16_t tot_alpn_len = ntohs(*((u_int16_t*)&packet->payload[s_offset]));
	  char alpn_str[256];
	  u_int8_t alpn_str_len = 0, i;

#ifdef DEBUG_TLS
	  printf("Server TLS [ALPN: block_len=%u/len=%u]\n", extension_len, tot_alpn_len);
#endif
	  s_offset += 2;
	  tot_alpn_len += s_offset;

	  while(s_offset < tot_alpn_len && s_offset < total_len) {
	    u_int8_t alpn_i, alpn_len = packet->payload[s_offset++];

	    if((s_offset + alpn_len) <= tot_alpn_len) {
#ifdef DEBUG_TLS
	      printf("Server TLS [ALPN: %u]\n", alpn_len);
#endif

	      if((alpn_str_len+alpn_len+1) < (sizeof(alpn_str)-1)) {
	        if(alpn_str_len > 0) {
	          alpn_str[alpn_str_len] = ',';
	          alpn_str_len++;
	        }

	        for(alpn_i=0; alpn_i<alpn_len; alpn_i++)
	        {
	          alpn_str[alpn_str_len+alpn_i] = packet->payload[s_offset+alpn_i];
	        }

	        s_offset += alpn_len, alpn_str_len += alpn_len;;
	      } else {
	        ndpi_set_risk(flow, NDPI_TLS_UNCOMMON_ALPN);
	        break;
	      }
	    } else {
	      ndpi_set_risk(flow, NDPI_TLS_UNCOMMON_ALPN);
	      break;
	    }
	  } /* while */

	  alpn_str[alpn_str_len] = '\0';

#ifdef DEBUG_TLS
	  printf("Server TLS [ALPN: %s][len: %u]\n", alpn_str, alpn_str_len);
#endif
	  if(flow->protos.tls_quic_stun.tls_quic.alpn == NULL)
	    flow->protos.tls_quic_stun.tls_quic.alpn = ndpi_strdup(alpn_str);

	  if(flow->protos.tls_quic_stun.tls_quic.alpn != NULL)
	    tlsCheckUncommonALPN(flow);

	  snprintf(ja3.server.alpn, sizeof(ja3.server.alpn), "%s", alpn_str);

	  /* Replace , with - as in JA3 */
	  for(i=0; ja3.server.alpn[i] != '\0'; i++)
	    if(ja3.server.alpn[i] == ',') ja3.server.alpn[i] = '-';
	} else if(extension_id == 11 /* ec_point_formats groups */) {
	  u_int16_t s_offset = offset+4 + 1;

#ifdef DEBUG_TLS
	  printf("Server TLS [EllipticCurveFormat: len=%u]\n", extension_len);
#endif
	  if((s_offset+extension_len-1) <= total_len) {
	    for(i=0; i<extension_len-1; i++) {
	      u_int8_t s_group = packet->payload[s_offset+i];

#ifdef DEBUG_TLS
	      printf("Server TLS [EllipticCurveFormat: %u]\n", s_group);
#endif

	      if(ja3.server.num_elliptic_curve_point_format < MAX_NUM_JA3)
		ja3.server.elliptic_curve_point_format[ja3.server.num_elliptic_curve_point_format++] = s_group;
	      else {
		invalid_ja3 = 1;
#ifdef DEBUG_TLS
		printf("Server TLS Invalid num elliptic %u\n", ja3.server.num_elliptic_curve_point_format);
#endif
	      }
	    }
	  } else {
	    invalid_ja3 = 1;
#ifdef DEBUG_TLS
	    printf("Server TLS Invalid len %u vs %u\n", s_offset+extension_len, total_len);
#endif
	  }
	}

	i += 4 + extension_len, offset += 4 + extension_len;
      } /* for */

      ja3_str_len = snprintf(ja3_str, sizeof(ja3_str), "%u,", ja3.server.tls_handshake_version);

      for(i=0; i<ja3.server.num_cipher; i++) {
	rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, "%s%u", (i > 0) ? "-" : "", ja3.server.cipher[i]);

	if(rc <= 0) break; else ja3_str_len += rc;
      }

      rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, ",");
      if(rc > 0 && ja3_str_len + rc < JA3_STR_LEN) ja3_str_len += rc;

      /* ********** */

      for(i=0; i<ja3.server.num_tls_extension; i++) {
	int rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, "%s%u", (i > 0) ? "-" : "", ja3.server.tls_extension[i]);

	if(rc <= 0) break; else ja3_str_len += rc;
      }

      if(ndpi_struct->enable_ja3_plus) {
	for(i=0; i<ja3.server.num_elliptic_curve_point_format; i++) {
	  rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, "%s%u",
			(i > 0) ? "-" : "", ja3.server.elliptic_curve_point_format[i]);
	  if((rc > 0) && (ja3_str_len + rc < JA3_STR_LEN)) ja3_str_len += rc; else break;
	}

	if(ja3.server.alpn[0] != '\0') {
	  rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, ",%s", ja3.server.alpn);
	  if((rc > 0) && (ja3_str_len + rc < JA3_STR_LEN)) ja3_str_len += rc;
	}

#ifdef DEBUG_TLS
	printf("[JA3+] Server: %s \n", ja3_str);
#endif
      } else {
#ifdef DEBUG_TLS
	printf("[JA3] Server: %s \n", ja3_str);
#endif
      }

      ndpi_MD5Init(&ctx);
      ndpi_MD5Update(&ctx, (const unsigned char *)ja3_str, strlen(ja3_str));
      ndpi_MD5Final(md5_hash, &ctx);

      for(i=0, j=0; i<16; i++) {
	int rc = snprintf(&flow->protos.tls_quic_stun.tls_quic.ja3_server[j],
			  sizeof(flow->protos.tls_quic_stun.tls_quic.ja3_server)-j, "%02x", md5_hash[i]);
	if(rc <= 0) break; else j += rc;
      }

#ifdef DEBUG_TLS
      printf("[JA3] Server: %s \n", flow->protos.tls_quic_stun.tls_quic.ja3_server);
#endif
    } else if(handshake_type == 0x01 /* Client Hello */) {
      u_int16_t cipher_len, cipher_offset;
      u_int8_t cookie_len = 0;

      flow->protos.tls_quic_stun.tls_quic.ssl_version = ja3.client.tls_handshake_version = tls_version;
      if(flow->protos.tls_quic_stun.tls_quic.ssl_version < 0x0302) /* TLSv1.1 */
	ndpi_set_risk(flow, NDPI_TLS_OBSOLETE_VERSION);

      if((session_id_len+base_offset+3) > packet->payload_packet_len)
	return(0); /* Not found */

      if(!is_dtls) {
	cipher_len = packet->payload[session_id_len+base_offset+2] + (packet->payload[session_id_len+base_offset+1] << 8);
	cipher_offset = base_offset + session_id_len + 3;
      } else {
	cookie_len = packet->payload[base_offset+session_id_len+1];
#ifdef DEBUG_TLS
	printf("[JA3] Client: DTLS cookie len %d\n", cookie_len);
#endif
	if((session_id_len+base_offset+cookie_len+4) > packet->payload_packet_len)
	  return(0); /* Not found */
	cipher_len = ntohs(*((u_int16_t*)&packet->payload[base_offset+session_id_len+cookie_len+2]));
	cipher_offset = base_offset + session_id_len + cookie_len + 4;
      }

#ifdef DEBUG_TLS
      printf("Client TLS [client cipher_len: %u][tls_version: 0x%04X]\n", cipher_len, tls_version);
#endif

      if((cipher_offset+cipher_len) <= total_len) {
	u_int8_t safari_ciphers = 0, chrome_ciphers = 0;

	for(i=0; i<cipher_len;) {
	  u_int16_t *id = (u_int16_t*)&packet->payload[cipher_offset+i];

#ifdef DEBUG_TLS
	  printf("Client TLS [cipher suite: %u/0x%04X] [%d/%u]\n", ntohs(*id), ntohs(*id), i, cipher_len);
#endif
	  if((*id == 0) || (packet->payload[cipher_offset+i] != packet->payload[cipher_offset+i+1])) {
	    u_int16_t cipher_id = ntohs(*id);
	    /*
	      Skip GREASE [https://tools.ietf.org/id/draft-ietf-tls-grease-01.html]
	      https://engineering.salesforce.com/tls-fingerprinting-with-ja3-and-ja3s-247362855967
	    */

	    if(ja3.client.num_cipher < MAX_NUM_JA3)
	      ja3.client.cipher[ja3.client.num_cipher++] = cipher_id;
	    else {
	      invalid_ja3 = 1;
#ifdef DEBUG_TLS
	      printf("Client TLS Invalid cipher %u\n", ja3.client.num_cipher);
#endif
	    }

	    switch(cipher_id) {
	    case TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256:
	    case TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384:
	      safari_ciphers++;
	      break;

	    case TLS_CIPHER_GREASE_RESERVED_0:
	    case TLS_AES_128_GCM_SHA256:
	    case TLS_AES_256_GCM_SHA384:
	    case TLS_CHACHA20_POLY1305_SHA256:
	      chrome_ciphers++;
	      break;

	    case TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256:
	    case TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384:
	    case TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256:
	    case TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256:
	    case TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA:
	    case TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA:
	    case TLS_RSA_WITH_AES_128_CBC_SHA:
	    case TLS_RSA_WITH_AES_256_CBC_SHA:
	    case TLS_RSA_WITH_AES_128_GCM_SHA256:
	    case TLS_RSA_WITH_AES_256_GCM_SHA384:
	      safari_ciphers++, chrome_ciphers++;
	      break;
	    }
	  }

	  i += 2;
	} /* for */

	if(chrome_ciphers == 13)
	  flow->protos.tls_quic_stun.tls_quic.browser_euristics.is_chrome_tls = 1;
	else if(safari_ciphers == 12)
	  flow->protos.tls_quic_stun.tls_quic.browser_euristics.is_safari_tls = 1;
      } else {
	invalid_ja3 = 1;
#ifdef DEBUG_TLS
	printf("Client TLS Invalid len %u vs %u\n", (cipher_offset+cipher_len), total_len);
#endif
      }

      offset = base_offset + session_id_len + cookie_len + cipher_len + 2;
      offset += (!is_dtls) ? 1 : 2;

      if(offset < total_len) {
	u_int16_t compression_len;
	u_int16_t extensions_len;

	compression_len = packet->payload[offset];
	offset++;

#ifdef DEBUG_TLS
	printf("Client TLS [compression_len: %u]\n", compression_len);
#endif

	// offset += compression_len + 3;
	offset += compression_len;

	if(offset+1 < total_len) {
	  extensions_len = ntohs(*((u_int16_t*)&packet->payload[offset]));
	  offset += 2;

#ifdef DEBUG_TLS
	  printf("Client TLS [extensions_len: %u]\n", extensions_len);
#endif

	  if((extensions_len+offset) <= total_len) {
	    /* Move to the first extension
	       Type is u_int to avoid possible overflow on extension_len addition */
	    u_int extension_offset = 0;
	    u_int32_t j;

	    while(extension_offset < extensions_len &&
		  offset+extension_offset+4 <= total_len) {
	      u_int16_t extension_id, extension_len, extn_off = offset+extension_offset;


	      extension_id = ntohs(*((u_int16_t*)&packet->payload[offset+extension_offset]));
	      extension_offset += 2;

	      extension_len = ntohs(*((u_int16_t*)&packet->payload[offset+extension_offset]));
	      extension_offset += 2;

#ifdef DEBUG_TLS
	      printf("Client TLS [extension_id: %u][extension_len: %u]\n", extension_id, extension_len);
#endif

	      if((extension_id == 0) || (packet->payload[extn_off] != packet->payload[extn_off+1])) {
		/* Skip GREASE */

		if(ja3.client.num_tls_extension < MAX_NUM_JA3)
		  ja3.client.tls_extension[ja3.client.num_tls_extension++] = extension_id;
		else {
		  invalid_ja3 = 1;
#ifdef DEBUG_TLS
		  printf("Client TLS Invalid extensions %u\n", ja3.client.num_tls_extension);
#endif
		}
	      }

	      if(extension_id == 0 /* server name */) {
		u_int16_t len;

#ifdef DEBUG_TLS
		printf("[TLS] Extensions: found server name\n");
#endif
		if((offset+extension_offset+4) < packet->payload_packet_len) {

		  len = (packet->payload[offset+extension_offset+3] << 8) + packet->payload[offset+extension_offset+4];
		  len = (u_int)ndpi_min(len, sizeof(buffer)-1);

		  if((offset+extension_offset+5+len) <= packet->payload_packet_len) {
		    strncpy(buffer, (char*)&packet->payload[offset+extension_offset+5], len);
		    buffer[len] = '\0';

		    cleanupServerName(buffer, sizeof(buffer));

		    snprintf(flow->protos.tls_quic_stun.tls_quic.client_requested_server_name,
			     sizeof(flow->protos.tls_quic_stun.tls_quic.client_requested_server_name),
			     "%s", buffer);
#ifdef DEBUG_TLS
		    printf("[TLS] SNI: [%s]\n", buffer);
#endif
		    if(!is_quic) {
		      if(ndpi_match_hostname_protocol(ndpi_struct, flow, NDPI_PROTOCOL_TLS, buffer, strlen(buffer)))
		        flow->l4.tcp.tls.subprotocol_detected = 1;
		    } else {
		      if(ndpi_match_hostname_protocol(ndpi_struct, flow, NDPI_PROTOCOL_QUIC, buffer, strlen(buffer)))
		        flow->l4.tcp.tls.subprotocol_detected = 1;
		    }

		    if(ndpi_check_dga_name(ndpi_struct, flow,
					   flow->protos.tls_quic_stun.tls_quic.client_requested_server_name, 1)) {
		      char *sni = flow->protos.tls_quic_stun.tls_quic.client_requested_server_name;
		      int len = strlen(sni);

#ifdef DEBUG_TLS
		      printf("[TLS] SNI: (DGA) [%s]\n", flow->protos.tls_quic_stun.tls_quic.client_requested_server_name);
#endif

		      if((len >= 4)
		         /* Check if it ends in .com or .net */
		         && ((strcmp(&sni[len-4], ".com") == 0) || (strcmp(&sni[len-4], ".net") == 0))
		         && (strncmp(sni, "www.", 4) == 0)) /* Not starting with www.... */
		        ndpi_set_detected_protocol(ndpi_struct, flow, NDPI_PROTOCOL_TOR, NDPI_PROTOCOL_TLS);
		    } else {
#ifdef DEBUG_TLS
		      printf("[TLS] SNI: (NO DGA) [%s]\n", flow->protos.tls_quic_stun.tls_quic.client_requested_server_name);
#endif
		    }
		  } else {
#ifdef DEBUG_TLS
		    printf("[TLS] Extensions server len too short: %u vs %u\n",
			   offset+extension_offset+5+len,
			   packet->payload_packet_len);
#endif
		  }
		}
	      } else if(extension_id == 10 /* supported groups */) {
		u_int16_t s_offset = offset+extension_offset + 2;

#ifdef DEBUG_TLS
		printf("Client TLS [EllipticCurveGroups: len=%u]\n", extension_len);
#endif

		if((s_offset+extension_len-2) <= total_len) {
		  for(i=0; i<extension_len-2;) {
		    u_int16_t s_group = ntohs(*((u_int16_t*)&packet->payload[s_offset+i]));

#ifdef DEBUG_TLS
		    printf("Client TLS [EllipticCurve: %u/0x%04X]\n", s_group, s_group);
#endif
		    if((s_group == 0) || (packet->payload[s_offset+i] != packet->payload[s_offset+i+1])) {
		      /* Skip GREASE */
		      if(ja3.client.num_elliptic_curve < MAX_NUM_JA3)
			ja3.client.elliptic_curve[ja3.client.num_elliptic_curve++] = s_group;
		      else {
			invalid_ja3 = 1;
#ifdef DEBUG_TLS
			printf("Client TLS Invalid num elliptic %u\n", ja3.client.num_elliptic_curve);
#endif
		      }
		    }

		    i += 2;
		  }
		} else {
		  invalid_ja3 = 1;
#ifdef DEBUG_TLS
		  printf("Client TLS Invalid len %u vs %u\n", (s_offset+extension_len-1), total_len);
#endif
		}
	      } else if(extension_id == 11 /* ec_point_formats groups */) {
		u_int16_t s_offset = offset+extension_offset + 1;

#ifdef DEBUG_TLS
		printf("Client TLS [EllipticCurveFormat: len=%u]\n", extension_len);
#endif
		if((s_offset+extension_len-1) <= total_len) {
		  for(i=0; i<extension_len-1; i++) {
		    u_int8_t s_group = packet->payload[s_offset+i];

#ifdef DEBUG_TLS
		    printf("Client TLS [EllipticCurveFormat: %u]\n", s_group);
#endif

		    if(ja3.client.num_elliptic_curve_point_format < MAX_NUM_JA3)
		      ja3.client.elliptic_curve_point_format[ja3.client.num_elliptic_curve_point_format++] = s_group;
		    else {
		      invalid_ja3 = 1;
#ifdef DEBUG_TLS
		      printf("Client TLS Invalid num elliptic %u\n", ja3.client.num_elliptic_curve_point_format);
#endif
		    }
		  }
		} else {
		  invalid_ja3 = 1;
#ifdef DEBUG_TLS
		  printf("Client TLS Invalid len %u vs %u\n", s_offset+extension_len, total_len);
#endif
		}
	      } else if(extension_id == 13 /* signature algorithms */) {
		u_int16_t s_offset = offset+extension_offset, safari_signature_algorithms = 0, chrome_signature_algorithms = 0;
		u_int16_t tot_signature_algorithms_len = ntohs(*((u_int16_t*)&packet->payload[s_offset]));

#ifdef DEBUG_TLS
		printf("Client TLS [SIGNATURE_ALGORITHMS: block_len=%u/len=%u]\n", extension_len, tot_signature_algorithms_len);
#endif

		s_offset += 2;
		tot_signature_algorithms_len = ndpi_min((sizeof(ja3.client.signature_algorithms) / 2) - 1, tot_signature_algorithms_len);

#ifdef TLS_HANDLE_SIGNATURE_ALGORITMS
		flow->protos.tls_quic_stun.tls_quic.num_tls_signature_algorithms = ndpi_min(tot_signature_algorithms_len / 2, MAX_NUM_TLS_SIGNATURE_ALGORITHMS);

		memcpy(flow->protos.tls_quic_stun.tls_quic.client_signature_algorithms,
		       &packet->payload[s_offset], 2 /* 16 bit */*flow->protos.tls_quic_stun.tls_quic.num_tls_signature_algorithms);
#endif

		for(i=0; i<tot_signature_algorithms_len; i++) {
		  int rc = snprintf(&ja3.client.signature_algorithms[i*2], sizeof(ja3.client.signature_algorithms)-i*2, "%02X", packet->payload[s_offset+i]);

		  if(rc < 0) break;
		}

		for(i=0; i<tot_signature_algorithms_len; i+=2) {
		  u_int16_t cipher_id = (u_int16_t)ntohs(*((u_int16_t*)&packet->payload[s_offset+i]));

		  // printf("=>> %04X\n", cipher_id);

		  switch(cipher_id) {
		  case ECDSA_SECP521R1_SHA512:
		    flow->protos.tls_quic_stun.tls_quic.browser_euristics.is_firefox_tls = 1;
		    break;

		  case ECDSA_SECP256R1_SHA256:
		  case ECDSA_SECP384R1_SHA384:
		  case RSA_PKCS1_SHA256:
		  case RSA_PKCS1_SHA384:
		  case RSA_PKCS1_SHA512:
		  case RSA_PSS_RSAE_SHA256:
		  case RSA_PSS_RSAE_SHA384:
		  case RSA_PSS_RSAE_SHA512:
		    chrome_signature_algorithms++, safari_signature_algorithms++;
		    break;
		  }
		}

		if(flow->protos.tls_quic_stun.tls_quic.browser_euristics.is_firefox_tls)
		  flow->protos.tls_quic_stun.tls_quic.browser_euristics.is_safari_tls = 0,
		    flow->protos.tls_quic_stun.tls_quic.browser_euristics.is_chrome_tls = 0;

		if(safari_signature_algorithms != 8)
		   flow->protos.tls_quic_stun.tls_quic.browser_euristics.is_safari_tls = 0;

		if(chrome_signature_algorithms != 8)
		   flow->protos.tls_quic_stun.tls_quic.browser_euristics.is_chrome_tls = 0;

		ja3.client.signature_algorithms[i*2] = '\0';

#ifdef DEBUG_TLS
		printf("Client TLS [SIGNATURE_ALGORITHMS: %s]\n", ja3.client.signature_algorithms);
#endif
	      } else if(extension_id == 16 /* application_layer_protocol_negotiation */) {
		u_int16_t s_offset = offset+extension_offset;
		u_int16_t tot_alpn_len = ntohs(*((u_int16_t*)&packet->payload[s_offset]));
		char alpn_str[256];
		u_int8_t alpn_str_len = 0, i;

#ifdef DEBUG_TLS
		printf("Client TLS [ALPN: block_len=%u/len=%u]\n", extension_len, tot_alpn_len);
#endif
		s_offset += 2;
		tot_alpn_len += s_offset;

		while(s_offset < tot_alpn_len && s_offset < total_len) {
		  u_int8_t alpn_i, alpn_len = packet->payload[s_offset++];

		  if((s_offset + alpn_len) <= tot_alpn_len &&
		     (s_offset + alpn_len) <= total_len) {
#ifdef DEBUG_TLS
		    printf("Client TLS [ALPN: %u]\n", alpn_len);
#endif

		    if((alpn_str_len+alpn_len+1) < (sizeof(alpn_str)-1)) {
		      if(alpn_str_len > 0) {
			alpn_str[alpn_str_len] = ',';
			alpn_str_len++;
		      }

		      for(alpn_i=0; alpn_i<alpn_len; alpn_i++)
			alpn_str[alpn_str_len+alpn_i] = packet->payload[s_offset+alpn_i];

		      s_offset += alpn_len, alpn_str_len += alpn_len;;
		    } else
		      break;
		  } else
		    break;
		} /* while */

		alpn_str[alpn_str_len] = '\0';

#ifdef DEBUG_TLS
		printf("Client TLS [ALPN: %s][len: %u]\n", alpn_str, alpn_str_len);
#endif
		if(flow->protos.tls_quic_stun.tls_quic.alpn == NULL)
		  flow->protos.tls_quic_stun.tls_quic.alpn = ndpi_strdup(alpn_str);

		snprintf(ja3.client.alpn, sizeof(ja3.client.alpn), "%s", alpn_str);

		/* Replace , with - as in JA3 */
		for(i=0; ja3.client.alpn[i] != '\0'; i++)
		  if(ja3.client.alpn[i] == ',') ja3.client.alpn[i] = '-';

	      } else if(extension_id == 43 /* supported versions */) {
		u_int16_t s_offset = offset+extension_offset;
		u_int8_t version_len = packet->payload[s_offset];
		char version_str[256];
		u_int8_t version_str_len = 0;
		version_str[0] = 0;
#ifdef DEBUG_TLS
		printf("Client TLS [TLS version len: %u]\n", version_len);
#endif

		if(version_len == (extension_len-1)) {
		  u_int8_t j;
		  u_int16_t supported_versions_offset = 0;

		  s_offset++;

		  // careful not to overflow and loop forever with u_int8_t
		  for(j=0; j+1<version_len; j += 2) {
		    u_int16_t tls_version = ntohs(*((u_int16_t*)&packet->payload[s_offset+j]));
		    u_int8_t unknown_tls_version;

#ifdef DEBUG_TLS
		    printf("Client TLS [TLS version: %s/0x%04X]\n",
			   ndpi_ssl_version2str(flow, tls_version, &unknown_tls_version), tls_version);
#endif

		    if((version_str_len+8) < sizeof(version_str)) {
		      int rc = snprintf(&version_str[version_str_len],
					sizeof(version_str) - version_str_len, "%s%s",
					(version_str_len > 0) ? "," : "",
					ndpi_ssl_version2str(flow, tls_version, &unknown_tls_version));
		      if(rc <= 0)
			break;
		      else
			version_str_len += rc;

		      rc = snprintf(&ja3.client.supported_versions[supported_versions_offset],
				    sizeof(ja3.client.supported_versions)-supported_versions_offset,
				    "%s%04X", (j > 0) ? "-" : "", tls_version);

		      if(rc > 0)
			supported_versions_offset += rc;
		    }
		  }

#ifdef DEBUG_TLS
		  printf("Client TLS [SUPPORTED_VERSIONS: %s]\n", ja3.client.supported_versions);
#endif

		  if(flow->protos.tls_quic_stun.tls_quic.tls_supported_versions == NULL)
		    flow->protos.tls_quic_stun.tls_quic.tls_supported_versions = ndpi_strdup(version_str);
		}
	      } else if(extension_id == 65486 /* encrypted server name */) {
		/*
		   - https://tools.ietf.org/html/draft-ietf-tls-esni-06
		   - https://blog.cloudflare.com/encrypted-sni/
		*/
		u_int16_t e_offset = offset+extension_offset;
		u_int16_t initial_offset = e_offset;
		u_int16_t e_sni_len, cipher_suite = ntohs(*((u_int16_t*)&packet->payload[e_offset]));

		flow->protos.tls_quic_stun.tls_quic.encrypted_sni.cipher_suite = cipher_suite;

		e_offset += 2; /* Cipher suite len */

		/* Key Share Entry */
		e_offset += 2; /* Group */
		e_offset +=  ntohs(*((u_int16_t*)&packet->payload[e_offset])) + 2; /* Lenght */

		if((e_offset+4) < packet->payload_packet_len) {
		  /* Record Digest */
		  e_offset +=  ntohs(*((u_int16_t*)&packet->payload[e_offset])) + 2; /* Lenght */

		  if((e_offset+4) < packet->payload_packet_len) {
		    e_sni_len = ntohs(*((u_int16_t*)&packet->payload[e_offset]));
		    e_offset += 2;

		    if((e_offset+e_sni_len-extension_len-initial_offset) >= 0 &&
		        e_offset+e_sni_len < packet->payload_packet_len) {
#ifdef DEBUG_ENCRYPTED_SNI
		      printf("Client TLS [Encrypted Server Name len: %u]\n", e_sni_len);
#endif

		      if(flow->protos.tls_quic_stun.tls_quic.encrypted_sni.esni == NULL) {
			flow->protos.tls_quic_stun.tls_quic.encrypted_sni.esni = (char*)ndpi_malloc(e_sni_len*2+1);

			if(flow->protos.tls_quic_stun.tls_quic.encrypted_sni.esni) {
			  u_int16_t i, off;

			  for(i=e_offset, off=0; i<(e_offset+e_sni_len); i++) {
			    int rc = sprintf(&flow->protos.tls_quic_stun.tls_quic.encrypted_sni.esni[off], "%02X", packet->payload[i] & 0XFF);

			    if(rc <= 0) {
			      flow->protos.tls_quic_stun.tls_quic.encrypted_sni.esni[off] = '\0';
			      break;
			    } else
			      off += rc;
			  }
			}
		      }
		    }
		  }
		}
	      } else if(extension_id == 65445 || /* QUIC transport parameters (drafts version) */
		        extension_id == 57) { /* QUIC transport parameters (final version) */
		u_int16_t s_offset = offset+extension_offset;
		uint16_t final_offset;
		int using_var_int = is_version_with_var_int_transport_params(quic_version);

		if(!using_var_int) {
		  if(s_offset+1 >= total_len) {
		    final_offset = 0; /* Force skipping extension */
		  } else {
		    u_int16_t seq_len = ntohs(*((u_int16_t*)&packet->payload[s_offset]));
		    s_offset += 2;
	            final_offset = MIN(total_len, s_offset + seq_len);
		  }
		} else {
	          final_offset = MIN(total_len, s_offset + extension_len);
		}

		while(s_offset < final_offset) {
		  u_int64_t param_type, param_len;

                  if(!using_var_int) {
		    if(s_offset+3 >= final_offset)
		      break;
		    param_type = ntohs(*((u_int16_t*)&packet->payload[s_offset]));
		    param_len = ntohs(*((u_int16_t*)&packet->payload[s_offset + 2]));
		    s_offset += 4;
		  } else {
		    if(s_offset >= final_offset ||
		       (s_offset + quic_len_buffer_still_required(packet->payload[s_offset])) >= final_offset)
		      break;
		    s_offset += quic_len(&packet->payload[s_offset], &param_type);

		    if(s_offset >= final_offset ||
		       (s_offset + quic_len_buffer_still_required(packet->payload[s_offset])) >= final_offset)
		      break;
		    s_offset += quic_len(&packet->payload[s_offset], &param_len);
		  }

#ifdef DEBUG_TLS
		  printf("Client TLS [QUIC TP: Param 0x%x Len %d]\n", (int)param_type, (int)param_len);
#endif
		  if(s_offset+param_len > final_offset)
		    break;

		  if(param_type==0x3129) {
#ifdef DEBUG_TLS
		      printf("UA [%.*s]\n", (int)param_len, &packet->payload[s_offset]);
#endif
		      http_process_user_agent(ndpi_struct, flow,
					      &packet->payload[s_offset], param_len);
		      break;
		  }
		  s_offset += param_len;
		}
	      }

	      extension_offset += extension_len; /* Move to the next extension */

#ifdef DEBUG_TLS
	      printf("Client TLS [extension_offset/len: %u/%u]\n", extension_offset, extension_len);
#endif
	    } /* while */

	    if(!invalid_ja3) {
	      int rc;

	    compute_ja3c:
	      ja3_str_len = snprintf(ja3_str, sizeof(ja3_str), "%u,", ja3.client.tls_handshake_version);

	      for(i=0; i<ja3.client.num_cipher; i++) {
		rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, "%s%u",
			      (i > 0) ? "-" : "", ja3.client.cipher[i]);
		if((rc > 0) && (ja3_str_len + rc < JA3_STR_LEN)) ja3_str_len += rc; else break;
	      }

	      rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, ",");
	      if((rc > 0) && (ja3_str_len + rc < JA3_STR_LEN)) ja3_str_len += rc;

	      /* ********** */

	      for(i=0; i<ja3.client.num_tls_extension; i++) {
		rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, "%s%u",
			      (i > 0) ? "-" : "", ja3.client.tls_extension[i]);
		if((rc > 0) && (ja3_str_len + rc < JA3_STR_LEN)) ja3_str_len += rc; else break;
	      }

	      rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, ",");
	      if((rc > 0) && (ja3_str_len + rc < JA3_STR_LEN)) ja3_str_len += rc;

	      /* ********** */

	      for(i=0; i<ja3.client.num_elliptic_curve; i++) {
		rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, "%s%u",
			      (i > 0) ? "-" : "", ja3.client.elliptic_curve[i]);
		if((rc > 0) && (ja3_str_len + rc < JA3_STR_LEN)) ja3_str_len += rc; else break;
	      }

	      rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, ",");
	      if((rc > 0) && (ja3_str_len + rc < JA3_STR_LEN)) ja3_str_len += rc;

	      for(i=0; i<ja3.client.num_elliptic_curve_point_format; i++) {
		rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, "%s%u",
			      (i > 0) ? "-" : "", ja3.client.elliptic_curve_point_format[i]);
		if((rc > 0) && (ja3_str_len + rc < JA3_STR_LEN)) ja3_str_len += rc; else break;
	      }

	      if(ndpi_struct->enable_ja3_plus) {
		rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len,
			      ",%s,%s,%s", ja3.client.signature_algorithms, ja3.client.supported_versions, ja3.client.alpn);
		if((rc > 0) && (ja3_str_len + rc < JA3_STR_LEN)) ja3_str_len += rc;
	      }

#ifdef DEBUG_JA3C
	      printf("[JA3+] Client: %s \n", ja3_str);
#endif

	      ndpi_MD5Init(&ctx);
	      ndpi_MD5Update(&ctx, (const unsigned char *)ja3_str, strlen(ja3_str));
	      ndpi_MD5Final(md5_hash, &ctx);

	      for(i=0, j=0; i<16; i++) {
		rc = snprintf(&flow->protos.tls_quic_stun.tls_quic.ja3_client[j],
			      sizeof(flow->protos.tls_quic_stun.tls_quic.ja3_client)-j, "%02x",
			      md5_hash[i]);
		if(rc > 0) j += rc; else break;
	      }

#ifdef DEBUG_JA3C
	      printf("[JA3] Client: %s \n", flow->protos.tls_quic_stun.tls_quic.ja3_client);
#endif

	      if(ndpi_struct->malicious_ja3_automa.ac_automa != NULL) {
		u_int16_t rc1 = ndpi_match_string(ndpi_struct->malicious_ja3_automa.ac_automa,
						  flow->protos.tls_quic_stun.tls_quic.ja3_client);

		if(rc1 > 0)
		  ndpi_set_risk(flow, NDPI_MALICIOUS_JA3);
	      }
	    }

	    /* Before returning to the caller we need to make a final check */
	    if((flow->protos.tls_quic_stun.tls_quic.ssl_version >= 0x0303) /* >= TLSv1.2 */
	       && (flow->protos.tls_quic_stun.tls_quic.alpn == NULL) /* No ALPN */) {
	      ndpi_set_risk(flow, NDPI_TLS_NOT_CARRYING_HTTPS);
	    }

	    /* Suspicious Domain Fronting:
	       https://github.com/SixGenInc/Noctilucent/blob/master/docs/ */
	    if(flow->protos.tls_quic_stun.tls_quic.encrypted_sni.esni &&
	       flow->protos.tls_quic_stun.tls_quic.client_requested_server_name[0] != '\0') {
	      ndpi_set_risk(flow, NDPI_TLS_SUSPICIOUS_ESNI_USAGE);
	    }

	    /* Add check for missing SNI */
	    if((flow->protos.tls_quic_stun.tls_quic.client_requested_server_name[0] == 0)
	       && (flow->protos.tls_quic_stun.tls_quic.ssl_version >= 0x0302) /* TLSv1.1 */
	       && (flow->protos.tls_quic_stun.tls_quic.encrypted_sni.esni == NULL) /* No ESNI */
	       ) {
	      /* This is a bit suspicious */
	      ndpi_set_risk(flow, NDPI_TLS_MISSING_SNI);
	    }

	    return(2 /* Client Certificate */);
	  } else {
#ifdef DEBUG_TLS
	    printf("[TLS] Client: too short [%u vs %u]\n",
		   (extensions_len+offset), total_len);
#endif
	  }
	} else if(offset == total_len) {
	  /* TLS does not have extensions etc */
	  goto compute_ja3c;
	}
      } else {
#ifdef DEBUG_TLS
	printf("[JA3] Client: invalid length detected\n");
#endif
      }
    }
  }

  return(0); /* Not found */
}