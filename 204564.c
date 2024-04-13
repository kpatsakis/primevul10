int dtls1_send_server_hello(SSL *s)
	{
	unsigned char *buf;
	unsigned char *p,*d;
	int i;
	unsigned int sl;
	unsigned long l,Time;

	if (s->state == SSL3_ST_SW_SRVR_HELLO_A)
		{
		buf=(unsigned char *)s->init_buf->data;
		p=s->s3->server_random;
		Time=(unsigned long)time(NULL);			/* Time */
		l2n(Time,p);
		RAND_pseudo_bytes(p,SSL3_RANDOM_SIZE-4);
		/* Do the message type and length last */
		d=p= &(buf[DTLS1_HM_HEADER_LENGTH]);

		if (s->client_version == DTLS1_BAD_VER)
			*(p++)=DTLS1_BAD_VER>>8,
			*(p++)=DTLS1_BAD_VER&0xff;
		else
			*(p++)=s->version>>8,
			*(p++)=s->version&0xff;

		/* Random stuff */
		memcpy(p,s->s3->server_random,SSL3_RANDOM_SIZE);
		p+=SSL3_RANDOM_SIZE;

		/* now in theory we have 3 options to sending back the
		 * session id.  If it is a re-use, we send back the
		 * old session-id, if it is a new session, we send
		 * back the new session-id or we send back a 0 length
		 * session-id if we want it to be single use.
		 * Currently I will not implement the '0' length session-id
		 * 12-Jan-98 - I'll now support the '0' length stuff.
		 */
		if (!(s->ctx->session_cache_mode & SSL_SESS_CACHE_SERVER))
			s->session->session_id_length=0;

		sl=s->session->session_id_length;
		if (sl > sizeof s->session->session_id)
			{
			SSLerr(SSL_F_DTLS1_SEND_SERVER_HELLO, ERR_R_INTERNAL_ERROR);
			return -1;
			}
		*(p++)=sl;
		memcpy(p,s->session->session_id,sl);
		p+=sl;

		/* put the cipher */
		if (s->s3->tmp.new_cipher == NULL)
			return -1;
		i=ssl3_put_cipher_by_char(s->s3->tmp.new_cipher,p);
		p+=i;

		/* put the compression method */
#ifdef OPENSSL_NO_COMP
		*(p++)=0;
#else
		if (s->s3->tmp.new_compression == NULL)
			*(p++)=0;
		else
			*(p++)=s->s3->tmp.new_compression->id;
#endif

#ifndef OPENSSL_NO_TLSEXT
		if ((p = ssl_add_serverhello_tlsext(s, p, buf+SSL3_RT_MAX_PLAIN_LENGTH)) == NULL)
			{
			SSLerr(SSL_F_DTLS1_SEND_SERVER_HELLO,ERR_R_INTERNAL_ERROR);
			return -1;
			}
#endif

		/* do the header */
		l=(p-d);
		d=buf;

		d = dtls1_set_message_header(s, d, SSL3_MT_SERVER_HELLO, l, 0, l);

		s->state=SSL3_ST_SW_SRVR_HELLO_B;
		/* number of bytes to write */
		s->init_num=p-buf;
		s->init_off=0;

		/* buffer the message to handle re-xmits */
		dtls1_buffer_message(s, 0);
		}

	/* SSL3_ST_SW_SRVR_HELLO_B */
	return(dtls1_do_write(s,SSL3_RT_HANDSHAKE));
	}