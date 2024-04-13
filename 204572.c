int dtls1_accept(SSL *s)
	{
	BUF_MEM *buf;
	unsigned long l,Time=(unsigned long)time(NULL);
	void (*cb)(const SSL *ssl,int type,int val)=NULL;
	int ret= -1;
	int new_state,state,skip=0;
	int listen;

	RAND_add(&Time,sizeof(Time),0);
	ERR_clear_error();
	clear_sys_error();

	if (s->info_callback != NULL)
		cb=s->info_callback;
	else if (s->ctx->info_callback != NULL)
		cb=s->ctx->info_callback;
	
	listen = s->d1->listen;

	/* init things to blank */
	s->in_handshake++;
	if (!SSL_in_init(s) || SSL_in_before(s)) SSL_clear(s);

	s->d1->listen = listen;

	if (s->cert == NULL)
		{
		SSLerr(SSL_F_DTLS1_ACCEPT,SSL_R_NO_CERTIFICATE_SET);
		return(-1);
		}

	for (;;)
		{
		state=s->state;

		switch (s->state)
			{
		case SSL_ST_RENEGOTIATE:
			s->new_session=1;
			/* s->state=SSL_ST_ACCEPT; */

		case SSL_ST_BEFORE:
		case SSL_ST_ACCEPT:
		case SSL_ST_BEFORE|SSL_ST_ACCEPT:
		case SSL_ST_OK|SSL_ST_ACCEPT:

			s->server=1;
			if (cb != NULL) cb(s,SSL_CB_HANDSHAKE_START,1);

			if ((s->version & 0xff00) != (DTLS1_VERSION & 0xff00))
				{
				SSLerr(SSL_F_DTLS1_ACCEPT, ERR_R_INTERNAL_ERROR);
				return -1;
				}
			s->type=SSL_ST_ACCEPT;

			if (s->init_buf == NULL)
				{
				if ((buf=BUF_MEM_new()) == NULL)
					{
					ret= -1;
					goto end;
					}
				if (!BUF_MEM_grow(buf,SSL3_RT_MAX_PLAIN_LENGTH))
					{
					ret= -1;
					goto end;
					}
				s->init_buf=buf;
				}

			if (!ssl3_setup_buffers(s))
				{
				ret= -1;
				goto end;
				}

			s->init_num=0;

			if (s->state != SSL_ST_RENEGOTIATE)
				{
				/* Ok, we now need to push on a buffering BIO so that
				 * the output is sent in a way that TCP likes :-)
				 */
				if (!ssl_init_wbio_buffer(s,1)) { ret= -1; goto end; }

				ssl3_init_finished_mac(s);
				s->state=SSL3_ST_SR_CLNT_HELLO_A;
				s->ctx->stats.sess_accept++;
				}
			else
				{
				/* s->state == SSL_ST_RENEGOTIATE,
				 * we will just send a HelloRequest */
				s->ctx->stats.sess_accept_renegotiate++;
				s->state=SSL3_ST_SW_HELLO_REQ_A;
				}

			break;

		case SSL3_ST_SW_HELLO_REQ_A:
		case SSL3_ST_SW_HELLO_REQ_B:

			s->shutdown=0;
			dtls1_clear_record_buffer(s);
			dtls1_start_timer(s);
			ret=dtls1_send_hello_request(s);
			if (ret <= 0) goto end;
			s->s3->tmp.next_state=SSL3_ST_SR_CLNT_HELLO_A;
			s->state=SSL3_ST_SW_FLUSH;
			s->init_num=0;

			ssl3_init_finished_mac(s);
			break;

		case SSL3_ST_SW_HELLO_REQ_C:
			s->state=SSL_ST_OK;
			break;

		case SSL3_ST_SR_CLNT_HELLO_A:
		case SSL3_ST_SR_CLNT_HELLO_B:
		case SSL3_ST_SR_CLNT_HELLO_C:

			s->shutdown=0;
			ret=ssl3_get_client_hello(s);
			if (ret <= 0) goto end;
			dtls1_stop_timer(s);

			if (ret == 1 && (SSL_get_options(s) & SSL_OP_COOKIE_EXCHANGE))
				s->state = DTLS1_ST_SW_HELLO_VERIFY_REQUEST_A;
			else
				s->state = SSL3_ST_SW_SRVR_HELLO_A;

			s->init_num=0;

			/* Reflect ClientHello sequence to remain stateless while listening */
			if (listen)
				{
				memcpy(s->s3->write_sequence, s->s3->read_sequence, sizeof(s->s3->write_sequence));
				}

			/* If we're just listening, stop here */
			if (listen && s->state == SSL3_ST_SW_SRVR_HELLO_A)
				{
				ret = 2;
				s->d1->listen = 0;
				/* Set expected sequence numbers
				 * to continue the handshake.
				 */
				s->d1->handshake_read_seq = 2;
				s->d1->handshake_write_seq = 1;
				s->d1->next_handshake_write_seq = 1;
				goto end;
				}
			
			break;
			
		case DTLS1_ST_SW_HELLO_VERIFY_REQUEST_A:
		case DTLS1_ST_SW_HELLO_VERIFY_REQUEST_B:

			ret = dtls1_send_hello_verify_request(s);
			if ( ret <= 0) goto end;
			s->state=SSL3_ST_SW_FLUSH;
			s->s3->tmp.next_state=SSL3_ST_SR_CLNT_HELLO_A;

			/* HelloVerifyRequests resets Finished MAC */
			if (s->client_version != DTLS1_BAD_VER)
				ssl3_init_finished_mac(s);
			break;
			
		case SSL3_ST_SW_SRVR_HELLO_A:
		case SSL3_ST_SW_SRVR_HELLO_B:
			s->new_session = 2;
			dtls1_start_timer(s);
			ret=dtls1_send_server_hello(s);
			if (ret <= 0) goto end;

#ifndef OPENSSL_NO_TLSEXT
			if (s->hit)
				{
				if (s->tlsext_ticket_expected)
					s->state=SSL3_ST_SW_SESSION_TICKET_A;
				else
					s->state=SSL3_ST_SW_CHANGE_A;
				}
#else
			if (s->hit)
					s->state=SSL3_ST_SW_CHANGE_A;
#endif
			else
				s->state=SSL3_ST_SW_CERT_A;
			s->init_num=0;
			break;

		case SSL3_ST_SW_CERT_A:
		case SSL3_ST_SW_CERT_B:
			/* Check if it is anon DH */
			if (!(s->s3->tmp.new_cipher->algorithms & SSL_aNULL))
				{
				dtls1_start_timer(s);
				ret=dtls1_send_server_certificate(s);
				if (ret <= 0) goto end;
#ifndef OPENSSL_NO_TLSEXT
				if (s->tlsext_status_expected)
					s->state=SSL3_ST_SW_CERT_STATUS_A;
				else
					s->state=SSL3_ST_SW_KEY_EXCH_A;
				}
			else
				{
				skip = 1;
				s->state=SSL3_ST_SW_KEY_EXCH_A;
				}
#else
				}
			else
				skip=1;

			s->state=SSL3_ST_SW_KEY_EXCH_A;
#endif
			s->init_num=0;
			break;

		case SSL3_ST_SW_KEY_EXCH_A:
		case SSL3_ST_SW_KEY_EXCH_B:
			l=s->s3->tmp.new_cipher->algorithms;

			/* clear this, it may get reset by
			 * send_server_key_exchange */
			s->s3->tmp.use_rsa_tmp=0;

			/* only send if a DH key exchange, fortezza or
			 * RSA but we have a sign only certificate */
			if ((l & (SSL_DH|SSL_kFZA))
			    || ((l & SSL_kRSA)
				&& (s->cert->pkeys[SSL_PKEY_RSA_ENC].privatekey == NULL
				    || (SSL_C_IS_EXPORT(s->s3->tmp.new_cipher)
					&& EVP_PKEY_size(s->cert->pkeys[SSL_PKEY_RSA_ENC].privatekey)*8 > SSL_C_EXPORT_PKEYLENGTH(s->s3->tmp.new_cipher)
					)
				    )
				)
			    )
				{
				dtls1_start_timer(s);
				ret=dtls1_send_server_key_exchange(s);
				if (ret <= 0) goto end;
				}
			else
				skip=1;

			s->state=SSL3_ST_SW_CERT_REQ_A;
			s->init_num=0;
			break;

		case SSL3_ST_SW_CERT_REQ_A:
		case SSL3_ST_SW_CERT_REQ_B:
			if (/* don't request cert unless asked for it: */
				!(s->verify_mode & SSL_VERIFY_PEER) ||
				/* if SSL_VERIFY_CLIENT_ONCE is set,
				 * don't request cert during re-negotiation: */
				((s->session->peer != NULL) &&
				 (s->verify_mode & SSL_VERIFY_CLIENT_ONCE)) ||
				/* never request cert in anonymous ciphersuites
				 * (see section "Certificate request" in SSL 3 drafts
				 * and in RFC 2246): */
				((s->s3->tmp.new_cipher->algorithms & SSL_aNULL) &&
				 /* ... except when the application insists on verification
				  * (against the specs, but s3_clnt.c accepts this for SSL 3) */
				 !(s->verify_mode & SSL_VERIFY_FAIL_IF_NO_PEER_CERT)) ||
                                 /* never request cert in Kerberos ciphersuites */
                                (s->s3->tmp.new_cipher->algorithms & SSL_aKRB5))
				{
				/* no cert request */
				skip=1;
				s->s3->tmp.cert_request=0;
				s->state=SSL3_ST_SW_SRVR_DONE_A;
				}
			else
				{
				s->s3->tmp.cert_request=1;
				dtls1_start_timer(s);
				ret=dtls1_send_certificate_request(s);
				if (ret <= 0) goto end;
#ifndef NETSCAPE_HANG_BUG
				s->state=SSL3_ST_SW_SRVR_DONE_A;
#else
				s->state=SSL3_ST_SW_FLUSH;
				s->s3->tmp.next_state=SSL3_ST_SR_CERT_A;
#endif
				s->init_num=0;
				}
			break;

		case SSL3_ST_SW_SRVR_DONE_A:
		case SSL3_ST_SW_SRVR_DONE_B:
			dtls1_start_timer(s);
			ret=dtls1_send_server_done(s);
			if (ret <= 0) goto end;
			s->s3->tmp.next_state=SSL3_ST_SR_CERT_A;
			s->state=SSL3_ST_SW_FLUSH;
			s->init_num=0;
			break;
		
		case SSL3_ST_SW_FLUSH:
			s->rwstate=SSL_WRITING;
			if (BIO_flush(s->wbio) <= 0)
				{
				ret= -1;
				goto end;
				}
			s->rwstate=SSL_NOTHING;
			s->state=s->s3->tmp.next_state;
			break;

		case SSL3_ST_SR_CERT_A:
		case SSL3_ST_SR_CERT_B:
			/* Check for second client hello (MS SGC) */
			ret = ssl3_check_client_hello(s);
			if (ret <= 0)
				goto end;
			if (ret == 2)
				{
				dtls1_stop_timer(s);
				s->state = SSL3_ST_SR_CLNT_HELLO_C;
				}
			else {
				if (s->s3->tmp.cert_request)
					{
					ret=ssl3_get_client_certificate(s);
					if (ret <= 0) goto end;
					}
				s->init_num=0;
				s->state=SSL3_ST_SR_KEY_EXCH_A;
			}
			break;

		case SSL3_ST_SR_KEY_EXCH_A:
		case SSL3_ST_SR_KEY_EXCH_B:
			ret=ssl3_get_client_key_exchange(s);
			if (ret <= 0) goto end;
			s->state=SSL3_ST_SR_CERT_VRFY_A;
			s->init_num=0;

			/* We need to get hashes here so if there is
			 * a client cert, it can be verified */ 
			s->method->ssl3_enc->cert_verify_mac(s,
				&(s->s3->finish_dgst1),
				&(s->s3->tmp.cert_verify_md[0]));
			s->method->ssl3_enc->cert_verify_mac(s,
				&(s->s3->finish_dgst2),
				&(s->s3->tmp.cert_verify_md[MD5_DIGEST_LENGTH]));

			break;

		case SSL3_ST_SR_CERT_VRFY_A:
		case SSL3_ST_SR_CERT_VRFY_B:

			s->d1->change_cipher_spec_ok = 1;
			/* we should decide if we expected this one */
			ret=ssl3_get_cert_verify(s);
			if (ret <= 0) goto end;

			s->state=SSL3_ST_SR_FINISHED_A;
			s->init_num=0;
			break;

		case SSL3_ST_SR_FINISHED_A:
		case SSL3_ST_SR_FINISHED_B:
			s->d1->change_cipher_spec_ok = 1;
			ret=ssl3_get_finished(s,SSL3_ST_SR_FINISHED_A,
				SSL3_ST_SR_FINISHED_B);
			if (ret <= 0) goto end;
			dtls1_stop_timer(s);
			if (s->hit)
				s->state=SSL_ST_OK;
#ifndef OPENSSL_NO_TLSEXT
			else if (s->tlsext_ticket_expected)
				s->state=SSL3_ST_SW_SESSION_TICKET_A;
#endif
			else
				s->state=SSL3_ST_SW_CHANGE_A;
			s->init_num=0;
			break;

#ifndef OPENSSL_NO_TLSEXT
		case SSL3_ST_SW_SESSION_TICKET_A:
		case SSL3_ST_SW_SESSION_TICKET_B:
			ret=dtls1_send_newsession_ticket(s);
			if (ret <= 0) goto end;
			s->state=SSL3_ST_SW_CHANGE_A;
			s->init_num=0;
			break;

		case SSL3_ST_SW_CERT_STATUS_A:
		case SSL3_ST_SW_CERT_STATUS_B:
			ret=ssl3_send_cert_status(s);
			if (ret <= 0) goto end;
			s->state=SSL3_ST_SW_KEY_EXCH_A;
			s->init_num=0;
			break;

#endif

		case SSL3_ST_SW_CHANGE_A:
		case SSL3_ST_SW_CHANGE_B:

			s->session->cipher=s->s3->tmp.new_cipher;
			if (!s->method->ssl3_enc->setup_key_block(s))
				{ ret= -1; goto end; }

			ret=dtls1_send_change_cipher_spec(s,
				SSL3_ST_SW_CHANGE_A,SSL3_ST_SW_CHANGE_B);

			if (ret <= 0) goto end;
			s->state=SSL3_ST_SW_FINISHED_A;
			s->init_num=0;

			if (!s->method->ssl3_enc->change_cipher_state(s,
				SSL3_CHANGE_CIPHER_SERVER_WRITE))
				{
				ret= -1;
				goto end;
				}

			dtls1_reset_seq_numbers(s, SSL3_CC_WRITE);
			break;

		case SSL3_ST_SW_FINISHED_A:
		case SSL3_ST_SW_FINISHED_B:
			ret=dtls1_send_finished(s,
				SSL3_ST_SW_FINISHED_A,SSL3_ST_SW_FINISHED_B,
				s->method->ssl3_enc->server_finished_label,
				s->method->ssl3_enc->server_finished_label_len);
			if (ret <= 0) goto end;
			s->state=SSL3_ST_SW_FLUSH;
			if (s->hit)
				s->s3->tmp.next_state=SSL3_ST_SR_FINISHED_A;
			else
				s->s3->tmp.next_state=SSL_ST_OK;
			s->init_num=0;
			break;

		case SSL_ST_OK:
			/* clean a few things up */
			ssl3_cleanup_key_block(s);

#if 0
			BUF_MEM_free(s->init_buf);
			s->init_buf=NULL;
#endif

			/* remove buffering on output */
			ssl_free_wbio_buffer(s);

			s->init_num=0;

			if (s->new_session == 2) /* skipped if we just sent a HelloRequest */
				{
				/* actually not necessarily a 'new' session unless
				 * SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION is set */
				
				s->new_session=0;
				
				ssl_update_cache(s,SSL_SESS_CACHE_SERVER);
				
				s->ctx->stats.sess_accept_good++;
				/* s->server=1; */
				s->handshake_func=dtls1_accept;

				if (cb != NULL) cb(s,SSL_CB_HANDSHAKE_DONE,1);
				}
			
			ret = 1;

			/* done handshaking, next message is client hello */
			s->d1->handshake_read_seq = 0;
			/* next message is server hello */
			s->d1->handshake_write_seq = 0;
			s->d1->next_handshake_write_seq = 0;
			goto end;
			/* break; */

		default:
			SSLerr(SSL_F_DTLS1_ACCEPT,SSL_R_UNKNOWN_STATE);
			ret= -1;
			goto end;
			/* break; */
			}
		
		if (!s->s3->tmp.reuse_message && !skip)
			{
			if (s->debug)
				{
				if ((ret=BIO_flush(s->wbio)) <= 0)
					goto end;
				}


			if ((cb != NULL) && (s->state != state))
				{
				new_state=s->state;
				s->state=state;
				cb(s,SSL_CB_ACCEPT_LOOP,1);
				s->state=new_state;
				}
			}
		skip=0;
		}