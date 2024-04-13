static int _gnutls_recv_supplemental(gnutls_session_t session)
{
	gnutls_buffer_st buf;
	int ret;

	_gnutls_debug_log("EXT[%p]: Expecting supplemental data\n",
			  session);

	ret =
	    _gnutls_recv_handshake(session, GNUTLS_HANDSHAKE_SUPPLEMENTAL,
				   1, &buf);
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}

	ret = _gnutls_parse_supplemental(session, buf.data, buf.length);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

      cleanup:
	_gnutls_buffer_clear(&buf);

	return ret;
}