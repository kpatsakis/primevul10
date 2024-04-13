int dtls1_send_server_certificate(SSL *s)
	{
	unsigned long l;
	X509 *x;

	if (s->state == SSL3_ST_SW_CERT_A)
		{
		x=ssl_get_server_send_cert(s);
		if (x == NULL &&
                        /* VRS: allow null cert if auth == KRB5 */
                        (s->s3->tmp.new_cipher->algorithms
                                & (SSL_MKEY_MASK|SSL_AUTH_MASK))
                        != (SSL_aKRB5|SSL_kKRB5))
			{
			SSLerr(SSL_F_DTLS1_SEND_SERVER_CERTIFICATE,ERR_R_INTERNAL_ERROR);
			return(0);
			}

		l=dtls1_output_cert_chain(s,x);
		s->state=SSL3_ST_SW_CERT_B;
		s->init_num=(int)l;
		s->init_off=0;

		/* buffer the message to handle re-xmits */
		dtls1_buffer_message(s, 0);
		}

	/* SSL3_ST_SW_CERT_B */
	return(dtls1_do_write(s,SSL3_RT_HANDSHAKE));
	}