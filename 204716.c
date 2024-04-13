unsigned char *ssl_add_serverhello_tlsext(SSL *s, unsigned char *p, unsigned char *limit)
	{
	int extdatalen=0;
	unsigned char *ret = p;
#ifndef OPENSSL_NO_NEXTPROTONEG
	int next_proto_neg_seen;
#endif
#ifndef OPENSSL_NO_EC
	unsigned long alg_k = s->s3->tmp.new_cipher->algorithm_mkey;
	unsigned long alg_a = s->s3->tmp.new_cipher->algorithm_auth;
	int using_ecc = (alg_k & (SSL_kEECDH|SSL_kECDHr|SSL_kECDHe)) || (alg_a & SSL_aECDSA);
	using_ecc = using_ecc && (s->session->tlsext_ecpointformatlist != NULL);
#endif
	/* don't add extensions for SSLv3, unless doing secure renegotiation */
	if (s->version == SSL3_VERSION && !s->s3->send_connection_binding)
		return p;
	
	ret+=2;
	if (ret>=limit) return NULL; /* this really never occurs, but ... */

	if (!s->hit && s->servername_done == 1 && s->session->tlsext_hostname != NULL)
		{ 
		if ((long)(limit - ret - 4) < 0) return NULL; 

		s2n(TLSEXT_TYPE_server_name,ret);
		s2n(0,ret);
		}

	if(s->s3->send_connection_binding)
        {
          int el;
          
          if(!ssl_add_serverhello_renegotiate_ext(s, 0, &el, 0))
              {
              SSLerr(SSL_F_SSL_ADD_SERVERHELLO_TLSEXT, ERR_R_INTERNAL_ERROR);
              return NULL;
              }

          if((limit - p - 4 - el) < 0) return NULL;
          
          s2n(TLSEXT_TYPE_renegotiate,ret);
          s2n(el,ret);

          if(!ssl_add_serverhello_renegotiate_ext(s, ret, &el, el))
              {
              SSLerr(SSL_F_SSL_ADD_SERVERHELLO_TLSEXT, ERR_R_INTERNAL_ERROR);
              return NULL;
              }

          ret += el;
        }

#ifndef OPENSSL_NO_EC
	if (using_ecc)
		{
		const unsigned char *plist;
		size_t plistlen;
		/* Add TLS extension ECPointFormats to the ServerHello message */
		long lenmax; 

		tls1_get_formatlist(s, &plist, &plistlen);

		if ((lenmax = limit - ret - 5) < 0) return NULL; 
		if (plistlen > (size_t)lenmax) return NULL;
		if (plistlen > 255)
			{
			SSLerr(SSL_F_SSL_ADD_SERVERHELLO_TLSEXT, ERR_R_INTERNAL_ERROR);
			return NULL;
			}
		
		s2n(TLSEXT_TYPE_ec_point_formats,ret);
		s2n(plistlen + 1,ret);
		*(ret++) = (unsigned char) plistlen;
		memcpy(ret, plist, plistlen);
		ret+=plistlen;

		}
	/* Currently the server should not respond with a SupportedCurves extension */
#endif /* OPENSSL_NO_EC */

	if (s->tlsext_ticket_expected
		&& !(SSL_get_options(s) & SSL_OP_NO_TICKET)) 
		{ 
		if ((long)(limit - ret - 4) < 0) return NULL; 
		s2n(TLSEXT_TYPE_session_ticket,ret);
		s2n(0,ret);
		}

	if (s->tlsext_status_expected)
		{ 
		if ((long)(limit - ret - 4) < 0) return NULL; 
		s2n(TLSEXT_TYPE_status_request,ret);
		s2n(0,ret);
		}

#ifdef TLSEXT_TYPE_opaque_prf_input
	if (s->s3->server_opaque_prf_input != NULL)
		{
		size_t sol = s->s3->server_opaque_prf_input_len;
		
		if ((long)(limit - ret - 6 - sol) < 0)
			return NULL;
		if (sol > 0xFFFD) /* can't happen */
			return NULL;

		s2n(TLSEXT_TYPE_opaque_prf_input, ret); 
		s2n(sol + 2, ret);
		s2n(sol, ret);
		memcpy(ret, s->s3->server_opaque_prf_input, sol);
		ret += sol;
		}
#endif

        if(s->srtp_profile)
                {
                int el;

                ssl_add_serverhello_use_srtp_ext(s, 0, &el, 0);
                
                if((limit - p - 4 - el) < 0) return NULL;

                s2n(TLSEXT_TYPE_use_srtp,ret);
                s2n(el,ret);

                if(ssl_add_serverhello_use_srtp_ext(s, ret, &el, el))
			{
			SSLerr(SSL_F_SSL_ADD_SERVERHELLO_TLSEXT, ERR_R_INTERNAL_ERROR);
			return NULL;
			}
                ret+=el;
                }

	if (((s->s3->tmp.new_cipher->id & 0xFFFF)==0x80 || (s->s3->tmp.new_cipher->id & 0xFFFF)==0x81) 
		&& (SSL_get_options(s) & SSL_OP_CRYPTOPRO_TLSEXT_BUG))
		{ const unsigned char cryptopro_ext[36] = {
			0xfd, 0xe8, /*65000*/
			0x00, 0x20, /*32 bytes length*/
			0x30, 0x1e, 0x30, 0x08, 0x06, 0x06, 0x2a, 0x85, 
			0x03,   0x02, 0x02, 0x09, 0x30, 0x08, 0x06, 0x06, 
			0x2a, 0x85, 0x03, 0x02, 0x02, 0x16, 0x30, 0x08, 
			0x06, 0x06, 0x2a, 0x85, 0x03, 0x02, 0x02, 0x17};
			if (limit-ret<36) return NULL;
			memcpy(ret,cryptopro_ext,36);
			ret+=36;

		}

#ifndef OPENSSL_NO_HEARTBEATS
	/* Add Heartbeat extension if we've received one */
	if (s->tlsext_heartbeat & SSL_TLSEXT_HB_ENABLED)
		{
		s2n(TLSEXT_TYPE_heartbeat,ret);
		s2n(1,ret);
		/* Set mode:
		 * 1: peer may send requests
		 * 2: peer not allowed to send requests
		 */
		if (s->tlsext_heartbeat & SSL_TLSEXT_HB_DONT_RECV_REQUESTS)
			*(ret++) = SSL_TLSEXT_HB_DONT_SEND_REQUESTS;
		else
			*(ret++) = SSL_TLSEXT_HB_ENABLED;

		}
#endif

#ifndef OPENSSL_NO_NEXTPROTONEG
	next_proto_neg_seen = s->s3->next_proto_neg_seen;
	s->s3->next_proto_neg_seen = 0;
	if (next_proto_neg_seen && s->ctx->next_protos_advertised_cb)
		{
		const unsigned char *npa;
		unsigned int npalen;
		int r;

		r = s->ctx->next_protos_advertised_cb(s, &npa, &npalen, s->ctx->next_protos_advertised_cb_arg);
		if (r == SSL_TLSEXT_ERR_OK)
			{
			if ((long)(limit - ret - 4 - npalen) < 0) return NULL;
			s2n(TLSEXT_TYPE_next_proto_neg,ret);
			s2n(npalen,ret);
			memcpy(ret, npa, npalen);
			ret += npalen;
			s->s3->next_proto_neg_seen = 1;
			}
		}
#endif

	/* If custom types were sent in ClientHello, add ServerHello responses */
	if (s->s3->tlsext_custom_types_count)
		{
		size_t i;

		for (i = 0; i < s->s3->tlsext_custom_types_count; i++)
			{
			size_t j;
			custom_srv_ext_record *record;

			for (j = 0; j < s->ctx->custom_srv_ext_records_count; j++)
				{
				record = &s->ctx->custom_srv_ext_records[j];
				if (s->s3->tlsext_custom_types[i] == record->ext_type)
					{
					const unsigned char *out = NULL;
					unsigned short outlen = 0;
					int cb_retval = 0;

					/* NULL callback or -1 omits extension */
					if (!record->fn2)
						break;
					cb_retval = record->fn2(s, record->ext_type,
						    		&out, &outlen,
						    		record->arg);
					if (cb_retval == 0)
						return NULL; /* error */
					if (cb_retval == -1)
						break; /* skip this extension */
					if (limit < ret + 4 + outlen)
						return NULL;
					s2n(record->ext_type, ret);
					s2n(outlen, ret);
					memcpy(ret, out, outlen);
					ret += outlen;
					break;
					}
				}
			}
		}
#ifdef TLSEXT_TYPE_encrypt_then_mac
	if (s->s3->flags & TLS1_FLAGS_ENCRYPT_THEN_MAC)
		{
		/* Don't use encrypt_then_mac if AEAD: might want
		 * to disable for other ciphersuites too.
		 */
		if (s->s3->tmp.new_cipher->algorithm_mac == SSL_AEAD)
			s->s3->flags &= ~TLS1_FLAGS_ENCRYPT_THEN_MAC;
		else
			{
			s2n(TLSEXT_TYPE_encrypt_then_mac,ret);
			s2n(0,ret);
			}
		}
#endif

	if (s->s3->alpn_selected)
		{
		const unsigned char *selected = s->s3->alpn_selected;
		unsigned len = s->s3->alpn_selected_len;

		if ((long)(limit - ret - 4 - 2 - 1 - len) < 0)
			return NULL;
		s2n(TLSEXT_TYPE_application_layer_protocol_negotiation,ret);
		s2n(3 + len,ret);
		s2n(1 + len,ret);
		*ret++ = len;
		memcpy(ret, selected, len);
		ret += len;
		}

	if ((extdatalen = ret-p-2)== 0) 
		return p;

	s2n(extdatalen,p);
	return ret;
	}