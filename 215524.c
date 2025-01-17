int processClientServerHello(struct ndpi_detection_module_struct *ndpi_struct,
			     struct ndpi_flow_struct *flow) {
  struct ndpi_packet_struct *packet = &flow->packet;
  struct ja3_info ja3;
  u_int8_t invalid_ja3 = 0;
  u_int16_t tls_version, ja3_str_len;
  char ja3_str[JA3_STR_LEN];
  ndpi_MD5_CTX ctx;
  u_char md5_hash[16];
  int i;
  u_int16_t total_len;
  u_int8_t handshake_type;
  char buffer[64] = { '\0' };

#ifdef DEBUG_TLS
  printf("SSL %s() called\n", __FUNCTION__);
#endif

  memset(&ja3, 0, sizeof(ja3));

  handshake_type = packet->payload[0];
  total_len = (packet->payload[1] << 16) +  (packet->payload[2] << 8) + packet->payload[3];

  if((total_len > packet->payload_packet_len) || (packet->payload[1] != 0x0))
    return(0); /* Not found */

  total_len = packet->payload_packet_len;

  /* At least "magic" 3 bytes, null for string end, otherwise no need to waste cpu cycles */
  if(total_len > 4) {
    u_int16_t base_offset    = packet->tcp ? 38 : 46;
    u_int16_t version_offset = packet->tcp ? 4 : 12;
    u_int16_t offset = packet->tcp ? 38 : 46, extension_len, j;
    u_int8_t  session_id_len =  0;
    if (base_offset < total_len)
      session_id_len = packet->payload[base_offset];

#ifdef DEBUG_TLS
    printf("SSL [len: %u][handshake_type: %02X]\n", packet->payload_packet_len, handshake_type);
#endif

    tls_version = ntohs(*((u_int16_t*)&packet->payload[version_offset]));
    flow->protos.stun_ssl.ssl.ssl_version = ja3.tls_handshake_version = tls_version;
    if(flow->protos.stun_ssl.ssl.ssl_version < 0x0302) /* TLSv1.1 */
      NDPI_SET_BIT(flow->risk, NDPI_TLS_OBSOLETE_VERSION);

    if(handshake_type == 0x02 /* Server Hello */) {
      int i, rc;

#ifdef DEBUG_TLS
      printf("SSL Server Hello [version: 0x%04X]\n", tls_version);
#endif

      /*
	The server hello decides about the SSL version of this flow
	https://networkengineering.stackexchange.com/questions/55752/why-does-wireshark-show-version-tls-1-2-here-instead-of-tls-1-3
      */
      if(packet->udp)
	offset += 1;
      else {
	if(tls_version < 0x7F15 /* TLS 1.3 lacks of session id */)
	  offset += session_id_len+1;
      }

      if((offset+3) > packet->payload_packet_len)
	return(0); /* Not found */

      ja3.num_cipher = 1, ja3.cipher[0] = ntohs(*((u_int16_t*)&packet->payload[offset]));
      if((flow->protos.stun_ssl.ssl.server_unsafe_cipher = ndpi_is_safe_ssl_cipher(ja3.cipher[0])) == 1)
	NDPI_SET_BIT(flow->risk, NDPI_TLS_WEAK_CIPHER);

      flow->protos.stun_ssl.ssl.server_cipher = ja3.cipher[0];

#ifdef DEBUG_TLS
      printf("TLS [server][session_id_len: %u][cipher: %04X]\n", session_id_len, ja3.cipher[0]);
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

	if(offset >= (packet->payload_packet_len+4)) break;

	extension_id  = ntohs(*((u_int16_t*)&packet->payload[offset]));
	extension_len = ntohs(*((u_int16_t*)&packet->payload[offset+2]));

	if(ja3.num_tls_extension < MAX_NUM_JA3)
	  ja3.tls_extension[ja3.num_tls_extension++] = extension_id;

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

	    flow->protos.stun_ssl.ssl.ssl_version = tls_version;
	  }
	}

	i += 4 + extension_len, offset += 4 + extension_len;
      }

      ja3_str_len = snprintf(ja3_str, sizeof(ja3_str), "%u,", ja3.tls_handshake_version);

      for(i=0; i<ja3.num_cipher; i++) {
	rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, "%s%u", (i > 0) ? "-" : "", ja3.cipher[i]);

	if(rc <= 0) break; else ja3_str_len += rc;
      }

      rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, ",");
      if(rc > 0 && ja3_str_len + rc < JA3_STR_LEN) ja3_str_len += rc;

      /* ********** */

      for(i=0; i<ja3.num_tls_extension; i++) {
	int rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, "%s%u", (i > 0) ? "-" : "", ja3.tls_extension[i]);

	if(rc <= 0) break; else ja3_str_len += rc;
      }

#ifdef DEBUG_TLS
      printf("TLS [server] %s\n", ja3_str);
#endif

#ifdef DEBUG_TLS
      printf("[JA3] Server: %s \n", ja3_str);
#endif

      ndpi_MD5Init(&ctx);
      ndpi_MD5Update(&ctx, (const unsigned char *)ja3_str, strlen(ja3_str));
      ndpi_MD5Final(md5_hash, &ctx);

      for(i=0, j=0; i<16; i++) {
	int rc = snprintf(&flow->protos.stun_ssl.ssl.ja3_server[j],
			  sizeof(flow->protos.stun_ssl.ssl.ja3_server)-j, "%02x", md5_hash[i]);
	if(rc <= 0) break; else j += rc;
      }

#ifdef DEBUG_TLS
      printf("[JA3] Server: %s \n", flow->protos.stun_ssl.ssl.ja3_server);
#endif
    } else if(handshake_type == 0x01 /* Client Hello */) {
      u_int16_t cipher_len, cipher_offset;

      if((session_id_len+base_offset+3) > packet->payload_packet_len)
	return(0); /* Not found */

      if(packet->tcp) {
	cipher_len = packet->payload[session_id_len+base_offset+2] + (packet->payload[session_id_len+base_offset+1] << 8);
	cipher_offset = base_offset + session_id_len + 3;
      } else {
	cipher_len = ntohs(*((u_int16_t*)&packet->payload[base_offset+2]));
	cipher_offset = base_offset+4;
      }

#ifdef DEBUG_TLS
      printf("Client SSL [client cipher_len: %u][tls_version: 0x%04X]\n", cipher_len, tls_version);
#endif

      if((cipher_offset+cipher_len) <= total_len) {
	for(i=0; i<cipher_len;) {
	  u_int16_t *id = (u_int16_t*)&packet->payload[cipher_offset+i];

#ifdef DEBUG_TLS
	  printf("Client SSL [cipher suite: %u/0x%04X] [%d/%u]\n", ntohs(*id), ntohs(*id), i, cipher_len);
#endif
	  if((*id == 0) || (packet->payload[cipher_offset+i] != packet->payload[cipher_offset+i+1])) {
	    /*
	      Skip GREASE [https://tools.ietf.org/id/draft-ietf-tls-grease-01.html]
	      https://engineering.salesforce.com/tls-fingerprinting-with-ja3-and-ja3s-247362855967
	    */

	    if(ja3.num_cipher < MAX_NUM_JA3)
	      ja3.cipher[ja3.num_cipher++] = ntohs(*id);
	    else {
	      invalid_ja3 = 1;
#ifdef DEBUG_TLS
	      printf("Client SSL Invalid cipher %u\n", ja3.num_cipher);
#endif
	    }
	  }

	  i += 2;
	}
      } else {
	invalid_ja3 = 1;
#ifdef DEBUG_TLS
	printf("Client SSL Invalid len %u vs %u\n", (cipher_offset+cipher_len), total_len);
#endif
      }

      offset = base_offset + session_id_len + cipher_len + 2;

      if(offset < total_len) {
	u_int16_t compression_len;
	u_int16_t extensions_len;

	offset += packet->tcp ? 1 : 2;
	compression_len = packet->payload[offset];
	offset++;

#ifdef DEBUG_TLS
	printf("Client SSL [compression_len: %u]\n", compression_len);
#endif

	// offset += compression_len + 3;
	offset += compression_len;

	if(offset < total_len) {
	  extensions_len = ntohs(*((u_int16_t*)&packet->payload[offset]));
	  offset += 2;

#ifdef DEBUG_TLS
	  printf("Client SSL [extensions_len: %u]\n", extensions_len);
#endif

	  if((extensions_len+offset) <= total_len) {
	    /* Move to the first extension
	       Type is u_int to avoid possible overflow on extension_len addition */
	    u_int extension_offset = 0;
	    u_int32_t j;

	    while(extension_offset < extensions_len) {
	      u_int16_t extension_id, extension_len, extn_off = offset+extension_offset;

	      extension_id = ntohs(*((u_int16_t*)&packet->payload[offset+extension_offset]));
	      extension_offset += 2;

	      extension_len = ntohs(*((u_int16_t*)&packet->payload[offset+extension_offset]));
	      extension_offset += 2;

#ifdef DEBUG_TLS
	      printf("Client SSL [extension_id: %u][extension_len: %u]\n", extension_id, extension_len);
#endif

	      if((extension_id == 0) || (packet->payload[extn_off] != packet->payload[extn_off+1])) {
		/* Skip GREASE */

		if(ja3.num_tls_extension < MAX_NUM_JA3)
		  ja3.tls_extension[ja3.num_tls_extension++] = extension_id;
		else {
		  invalid_ja3 = 1;
#ifdef DEBUG_TLS
		  printf("Client SSL Invalid extensions %u\n", ja3.num_tls_extension);
#endif
		}
	      }

	      if(extension_id == 0 /* server name */) {
		u_int16_t len;

#ifdef DEBUG_TLS
		printf("[TLS] Extensions: found server name\n");
#endif

		len = (packet->payload[offset+extension_offset+3] << 8) + packet->payload[offset+extension_offset+4];
		len = (u_int)ndpi_min(len, sizeof(buffer)-1);

		if((offset+extension_offset+5+len) <= packet->payload_packet_len) {
		  strncpy(buffer, (char*)&packet->payload[offset+extension_offset+5], len);
		  buffer[len] = '\0';

		  cleanupServerName(buffer, sizeof(buffer));

		  snprintf(flow->protos.stun_ssl.ssl.client_requested_server_name,
			   sizeof(flow->protos.stun_ssl.ssl.client_requested_server_name),
			   "%s", buffer);

		  if(ndpi_match_hostname_protocol(ndpi_struct, flow, NDPI_PROTOCOL_TLS, buffer, strlen(buffer)))
		    flow->l4.tcp.tls.subprotocol_detected = 1;

		  ndpi_check_dga_name(ndpi_struct, flow, flow->protos.stun_ssl.ssl.client_requested_server_name);
		} else {
#ifdef DEBUG_TLS
		  printf("[TLS] Extensions server len too short: %u vs %u\n",
			 offset+extension_offset+5+len,
			 packet->payload_packet_len);
#endif
		}
	      } else if(extension_id == 10 /* supported groups */) {
		u_int16_t s_offset = offset+extension_offset + 2;

#ifdef DEBUG_TLS
		printf("Client SSL [EllipticCurveGroups: len=%u]\n", extension_len);
#endif

		if((s_offset+extension_len-2) <= total_len) {
		  for(i=0; i<extension_len-2;) {
		    u_int16_t s_group = ntohs(*((u_int16_t*)&packet->payload[s_offset+i]));

#ifdef DEBUG_TLS
		    printf("Client SSL [EllipticCurve: %u/0x%04X]\n", s_group, s_group);
#endif
		    if((s_group == 0) || (packet->payload[s_offset+i] != packet->payload[s_offset+i+1])) {
		      /* Skip GREASE */
		      if(ja3.num_elliptic_curve < MAX_NUM_JA3)
			ja3.elliptic_curve[ja3.num_elliptic_curve++] = s_group;
		      else {
			invalid_ja3 = 1;
#ifdef DEBUG_TLS
			printf("Client SSL Invalid num elliptic %u\n", ja3.num_elliptic_curve);
#endif
		      }
		    }

		    i += 2;
		  }
		} else {
		  invalid_ja3 = 1;
#ifdef DEBUG_TLS
		  printf("Client SSL Invalid len %u vs %u\n", (s_offset+extension_len-1), total_len);
#endif
		}
	      } else if(extension_id == 11 /* ec_point_formats groups */) {
		u_int16_t s_offset = offset+extension_offset + 1;

#ifdef DEBUG_TLS
		printf("Client SSL [EllipticCurveFormat: len=%u]\n", extension_len);
#endif
		if((s_offset+extension_len) < total_len) {
		  for(i=0; i<extension_len-1;i++) {
		    u_int8_t s_group = packet->payload[s_offset+i];

#ifdef DEBUG_TLS
		    printf("Client SSL [EllipticCurveFormat: %u]\n", s_group);
#endif

		    if(ja3.num_elliptic_curve_point_format < MAX_NUM_JA3)
		      ja3.elliptic_curve_point_format[ja3.num_elliptic_curve_point_format++] = s_group;
		    else {
		      invalid_ja3 = 1;
#ifdef DEBUG_TLS
		      printf("Client SSL Invalid num elliptic %u\n", ja3.num_elliptic_curve_point_format);
#endif
		    }
		  }
		} else {
		  invalid_ja3 = 1;
#ifdef DEBUG_TLS
		  printf("Client SSL Invalid len %u vs %u\n", s_offset+extension_len, total_len);
#endif
		}
	      } else if(extension_id == 16 /* application_layer_protocol_negotiation */) {
		u_int16_t s_offset = offset+extension_offset;
		u_int16_t tot_alpn_len = ntohs(*((u_int16_t*)&packet->payload[s_offset]));
		char alpn_str[256];
		u_int8_t alpn_str_len = 0;

#ifdef DEBUG_TLS
		printf("Client SSL [ALPN: block_len=%u/len=%u]\n", extension_len, tot_alpn_len);
#endif
		s_offset += 2;
		tot_alpn_len += s_offset;

		while(s_offset < tot_alpn_len && s_offset < total_len) {
		  u_int8_t alpn_i, alpn_len = packet->payload[s_offset++];

		  if((s_offset + alpn_len) <= tot_alpn_len) {
#ifdef DEBUG_TLS
		    printf("Client SSL [ALPN: %u]\n", alpn_len);
#endif

		    if((alpn_str_len+alpn_len+1) < sizeof(alpn_str)) {
		      if(alpn_str_len > 0) {
			alpn_str[alpn_str_len] = ',';
			alpn_str_len++;
		      }

		      for(alpn_i=0; alpn_i<alpn_len; alpn_i++)
			alpn_str[alpn_str_len+alpn_i] =  packet->payload[s_offset+alpn_i];

		      s_offset += alpn_len, alpn_str_len += alpn_len;;
		    } else
		      break;
		  } else
		    break;
		} /* while */

		alpn_str[alpn_str_len] = '\0';

#ifdef DEBUG_TLS
		printf("Client SSL [ALPN: %s][len: %u]\n", alpn_str, alpn_str_len);
#endif
		if(flow->protos.stun_ssl.ssl.alpn == NULL)
		  flow->protos.stun_ssl.ssl.alpn = ndpi_strdup(alpn_str);
	      } else if(extension_id == 43 /* supported versions */) {
		u_int16_t s_offset = offset+extension_offset;
		u_int8_t version_len = packet->payload[s_offset];
		char version_str[256];
		u_int8_t version_str_len = 0;
		version_str[0] = 0;
#ifdef DEBUG_TLS
		printf("Client SSL [TLS version len: %u]\n", version_len);
#endif

		if(version_len == (extension_len-1)) {
		  u_int8_t j;

		  s_offset++;

		  // careful not to overflow and loop forever with u_int8_t
		  for(j=0; j+1<version_len; j += 2) {
		    u_int16_t tls_version = ntohs(*((u_int16_t*)&packet->payload[s_offset+j]));
		    u_int8_t unknown_tls_version;

#ifdef DEBUG_TLS
		    printf("Client SSL [TLS version: %s/0x%04X]\n",
			   ndpi_ssl_version2str(tls_version, &unknown_tls_version), tls_version);
#endif

		    if((version_str_len+8) < sizeof(version_str)) {
		      int rc = snprintf(&version_str[version_str_len],
					sizeof(version_str) - version_str_len, "%s%s",
					(version_str_len > 0) ? "," : "",
					ndpi_ssl_version2str(tls_version, &unknown_tls_version));
		      if(rc <= 0)
			break;
		      else
			version_str_len += rc;
		    }
		  }
		if(flow->protos.stun_ssl.ssl.tls_supported_versions == NULL)
		  flow->protos.stun_ssl.ssl.tls_supported_versions = ndpi_strdup(version_str);
		}
	      } else if(extension_id == 65486 /* encrypted server name */) {
		/*
		   - https://tools.ietf.org/html/draft-ietf-tls-esni-06
		   - https://blog.cloudflare.com/encrypted-sni/
		*/
		u_int16_t e_offset = offset+extension_offset;
		u_int16_t initial_offset = e_offset;
		u_int16_t e_sni_len, cipher_suite = ntohs(*((u_int16_t*)&packet->payload[e_offset]));

		flow->protos.stun_ssl.ssl.encrypted_sni.cipher_suite = cipher_suite;

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

		    if((e_offset+e_sni_len-extension_len-initial_offset) >= 0) {
#ifdef DEBUG_ENCRYPTED_SNI
		      printf("Client SSL [Encrypted Server Name len: %u]\n", e_sni_len);
#endif

		      if(flow->protos.stun_ssl.ssl.encrypted_sni.esni == NULL) {
			flow->protos.stun_ssl.ssl.encrypted_sni.esni = (char*)ndpi_malloc(e_sni_len*2+1);

			if(flow->protos.stun_ssl.ssl.encrypted_sni.esni) {
			  u_int16_t i, off;

			  for(i=e_offset, off=0; i<(e_offset+e_sni_len); i++) {
			    int rc = sprintf(&flow->protos.stun_ssl.ssl.encrypted_sni.esni[off], "%02X", packet->payload[i] & 0XFF);

			    if(rc <= 0) {
			      flow->protos.stun_ssl.ssl.encrypted_sni.esni[off] = '\0';
			      break;
			    } else
			      off += rc;
			  }
			}
		      }
		    }
		  }
		}
	      }

	      extension_offset += extension_len; /* Move to the next extension */

#ifdef DEBUG_TLS
	      printf("Client SSL [extension_offset/len: %u/%u]\n", extension_offset, extension_len);
#endif
	    } /* while */

	    if(!invalid_ja3) {
	      int rc;

	    compute_ja3c:
	      ja3_str_len = snprintf(ja3_str, sizeof(ja3_str), "%u,", ja3.tls_handshake_version);

	      for(i=0; i<ja3.num_cipher; i++) {
		rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, "%s%u",
			      (i > 0) ? "-" : "", ja3.cipher[i]);
		if(rc > 0 && ja3_str_len + rc < JA3_STR_LEN) ja3_str_len += rc; else break;
	      }

	      rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, ",");
	      if(rc > 0 && ja3_str_len + rc < JA3_STR_LEN) ja3_str_len += rc;

	      /* ********** */

	      for(i=0; i<ja3.num_tls_extension; i++) {
		rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, "%s%u",
			      (i > 0) ? "-" : "", ja3.tls_extension[i]);
		if(rc > 0 && ja3_str_len + rc < JA3_STR_LEN) ja3_str_len += rc; else break;
	      }

	      rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, ",");
	      if(rc > 0 && ja3_str_len + rc < JA3_STR_LEN) ja3_str_len += rc;

	      /* ********** */

	      for(i=0; i<ja3.num_elliptic_curve; i++) {
		rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, "%s%u",
			      (i > 0) ? "-" : "", ja3.elliptic_curve[i]);
		if(rc > 0 && ja3_str_len + rc < JA3_STR_LEN) ja3_str_len += rc; else break;
	      }

	      rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, ",");
	      if(rc > 0 && ja3_str_len + rc < JA3_STR_LEN) ja3_str_len += rc;

	      for(i=0; i<ja3.num_elliptic_curve_point_format; i++) {
		rc = snprintf(&ja3_str[ja3_str_len], sizeof(ja3_str)-ja3_str_len, "%s%u",
			      (i > 0) ? "-" : "", ja3.elliptic_curve_point_format[i]);
		if(rc > 0 && ja3_str_len + rc < JA3_STR_LEN) ja3_str_len += rc; else break;
	      }

#ifdef DEBUG_TLS
	      printf("[JA3] Client: %s \n", ja3_str);
#endif

	      ndpi_MD5Init(&ctx);
	      ndpi_MD5Update(&ctx, (const unsigned char *)ja3_str, strlen(ja3_str));
	      ndpi_MD5Final(md5_hash, &ctx);

	      for(i=0, j=0; i<16; i++) {
		rc = snprintf(&flow->protos.stun_ssl.ssl.ja3_client[j],
			      sizeof(flow->protos.stun_ssl.ssl.ja3_client)-j, "%02x",
			      md5_hash[i]);
		if(rc > 0) j += rc; else break;
	      }
#ifdef DEBUG_TLS
	      printf("[JA3] Client: %s \n", flow->protos.stun_ssl.ssl.ja3_client);
#endif
	    }

	    /* Before returning to the caller we need to make a final check */
	    if((flow->protos.stun_ssl.ssl.ssl_version >= 0x0303) /* >= TLSv1.2 */
	       && (flow->protos.stun_ssl.ssl.alpn == NULL) /* No ALPN */) {
	      NDPI_SET_BIT(flow->risk, NDPI_TLS_NOT_CARRYING_HTTPS);
	    }

	    return(2 /* Client Certificate */);
	  } else {
#ifdef DEBUG_TLS
	    printf("[TLS] Client: too short [%u vs %u]\n",
		   (extensions_len+offset), total_len);
#endif
	  }
	} else if(offset == total_len) {
	  /* SSL does not have extensions etc */
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