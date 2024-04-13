static int _gnutls_send_supplemental(gnutls_session_t session, int again)
{
	mbuffer_st *bufel;
	int ret = 0;

	_gnutls_debug_log("EXT[%p]: Sending supplemental data\n", session);

	if (again)
		ret =
		    _gnutls_send_handshake(session, NULL,
					   GNUTLS_HANDSHAKE_SUPPLEMENTAL);
	else {
		gnutls_buffer_st buf;
		_gnutls_buffer_init(&buf);

		ret = _gnutls_gen_supplemental(session, &buf);
		if (ret < 0) {
			gnutls_assert();
			return ret;
		}

		bufel =
		    _gnutls_handshake_alloc(session, 
					    buf.length);
		if (bufel == NULL) {
			gnutls_assert();
			return GNUTLS_E_MEMORY_ERROR;
		}

		_mbuffer_set_udata(bufel, buf.data, buf.length);
		_gnutls_buffer_clear(&buf);

		ret = _gnutls_send_handshake(session, bufel,
					     GNUTLS_HANDSHAKE_SUPPLEMENTAL);
	}

	return ret;
}