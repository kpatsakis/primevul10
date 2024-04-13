_gnutls_send_handshake(gnutls_session_t session, mbuffer_st * bufel,
		       gnutls_handshake_description_t type)
{
	int ret;
	uint8_t *data;
	uint32_t datasize, i_datasize;
	int pos = 0;

	if (bufel == NULL) {
		/* we are resuming a previously interrupted
		 * send.
		 */
		ret = _gnutls_handshake_io_write_flush(session);
		return ret;

	}

	/* first run */
	data = _mbuffer_get_uhead_ptr(bufel);
	i_datasize = _mbuffer_get_udata_size(bufel);
	datasize = i_datasize + _mbuffer_get_uhead_size(bufel);

	data[pos++] = (uint8_t) type;
	_gnutls_write_uint24(_mbuffer_get_udata_size(bufel), &data[pos]);
	pos += 3;

	/* Add DTLS handshake fragment headers.  The message will be
	 * fragmented later by the fragmentation sub-layer. All fields must
	 * be set properly for HMAC. The HMAC requires we pretend that the
	 * message was sent in a single fragment. */
	if (IS_DTLS(session)) {
		_gnutls_write_uint16(session->internals.dtls.
				     hsk_write_seq++, &data[pos]);
		pos += 2;

		/* Fragment offset */
		_gnutls_write_uint24(0, &data[pos]);
		pos += 3;

		/* Fragment length */
		_gnutls_write_uint24(i_datasize, &data[pos]);
		/* pos += 3; */
	}

	_gnutls_handshake_log("HSK[%p]: %s was queued [%ld bytes]\n",
			      session, _gnutls_handshake2str(type),
			      (long) datasize);

	/* Here we keep the handshake messages in order to hash them...
	 */
	if (type != GNUTLS_HANDSHAKE_HELLO_REQUEST)
		if ((ret =
		     handshake_hash_add_sent(session, type, data,
						     datasize)) < 0) {
			gnutls_assert();
			_mbuffer_xfree(&bufel);
			return ret;
		}

	ret = call_hook_func(session, type, GNUTLS_HOOK_PRE, 0,
		             _mbuffer_get_udata_ptr(bufel), _mbuffer_get_udata_size(bufel));
	if (ret < 0) {
		gnutls_assert();
		_mbuffer_xfree(&bufel);
		return ret;
	}

	session->internals.last_handshake_out = type;

	ret = _gnutls_handshake_io_cache_int(session, type, bufel);
	if (ret < 0) {
		_mbuffer_xfree(&bufel);
		gnutls_assert();
		return ret;
	}

	ret = call_hook_func(session, type, GNUTLS_HOOK_POST, 0, 
	                      _mbuffer_get_udata_ptr(bufel), _mbuffer_get_udata_size(bufel));
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}

	switch (type) {
	case GNUTLS_HANDSHAKE_CERTIFICATE_PKT:	/* this one is followed by ServerHelloDone
						 * or ClientKeyExchange always.
						 */
	case GNUTLS_HANDSHAKE_CERTIFICATE_STATUS:
	case GNUTLS_HANDSHAKE_SERVER_KEY_EXCHANGE:	/* as above */
	case GNUTLS_HANDSHAKE_SERVER_HELLO:	/* as above */
	case GNUTLS_HANDSHAKE_CERTIFICATE_REQUEST:	/* as above */
	case GNUTLS_HANDSHAKE_NEW_SESSION_TICKET:	/* followed by ChangeCipherSpec */

		/* now for client Certificate, ClientKeyExchange and
		 * CertificateVerify are always followed by ChangeCipherSpec
		 */
	case GNUTLS_HANDSHAKE_CERTIFICATE_VERIFY:
	case GNUTLS_HANDSHAKE_CLIENT_KEY_EXCHANGE:
		ret = 0;
		break;
	default:
		/* send cached messages */
		ret = _gnutls_handshake_io_write_flush(session);
		break;
	}

	return ret;
}