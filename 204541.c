int ssl3_write_bytes(SSL *s, int type, const void *buf_, int len)
	{
	const unsigned char *buf=buf_;
	unsigned int tot,n,nw;
	int i;

	s->rwstate=SSL_NOTHING;
	tot=s->s3->wnum;
	s->s3->wnum=0;

	if (SSL_in_init(s) && !s->in_handshake)
		{
		i=s->handshake_func(s);
		if (i < 0) return(i);
		if (i == 0)
			{
			SSLerr(SSL_F_SSL3_WRITE_BYTES,SSL_R_SSL_HANDSHAKE_FAILURE);
			return -1;
			}
		}

	/* ensure that if we end up with a smaller value of data to write 
	 * out than the the original len from a write which didn't complete 
	 * for non-blocking I/O and also somehow ended up avoiding 
	 * the check for this in ssl3_write_pending/SSL_R_BAD_WRITE_RETRY as
	 * it must never be possible to end up with (len-tot) as a large
	 * number that will then promptly send beyond the end of the users
	 * buffer ... so we trap and report the error in a way the user
	 * will notice
	 */
	if ( len < tot)
		{
		SSLerr(SSL_F_SSL3_WRITE_BYTES,SSL_R_BAD_LENGTH);
		return(-1);
		}


	n=(len-tot);
	for (;;)
		{
		if (n > s->max_send_fragment)
			nw=s->max_send_fragment;
		else
			nw=n;

		i=do_ssl3_write(s, type, &(buf[tot]), nw, 0);
		if (i <= 0)
			{
			s->s3->wnum=tot;
			return i;
			}

		if ((i == (int)n) ||
			(type == SSL3_RT_APPLICATION_DATA &&
			 (s->mode & SSL_MODE_ENABLE_PARTIAL_WRITE)))
			{
			/* next chunk of data should get another prepended empty fragment
			 * in ciphersuites with known-IV weakness: */
			s->s3->empty_fragment_done = 0;
			
			return tot+i;
			}

		n-=i;
		tot+=i;
		}
	}