int ssl3_send_server_certificate(SSL *s)
	{
	unsigned long l;
	X509 *x;

	if (s->state == SSL3_ST_SW_CERT_A)
		{
		x=ssl_get_server_send_cert(s);
		if (x == NULL)
			{
			/* VRS: allow null cert if auth == KRB5 */
			if ((s->s3->tmp.new_cipher->algorithm_auth != SSL_aKRB5) ||
			    (s->s3->tmp.new_cipher->algorithm_mkey & SSL_kKRB5))
				{
				SSLerr(SSL_F_SSL3_SEND_SERVER_CERTIFICATE,ERR_R_INTERNAL_ERROR);
				return(0);
				}
			}

		l=ssl3_output_cert_chain(s,x);
		if (!l)
			{
			SSLerr(SSL_F_SSL3_SEND_SERVER_CERTIFICATE,ERR_R_INTERNAL_ERROR);
			return(0);
			}
		s->state=SSL3_ST_SW_CERT_B;
		s->init_num=(int)l;
		s->init_off=0;
		}

	/* SSL3_ST_SW_CERT_B */
	return(ssl3_do_write(s,SSL3_RT_HANDSHAKE));
	}