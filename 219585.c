_gnutls_recv_handshake(gnutls_session_t session,
		       gnutls_handshake_description_t type,
		       unsigned int optional, gnutls_buffer_st * buf)
{
	int ret, ret2;
	handshake_buffer_st hsk;

	ret = _gnutls_handshake_io_recv_int(session, type, &hsk, optional);
	if (ret < 0) {
		if (optional != 0
		    && ret == GNUTLS_E_UNEXPECTED_HANDSHAKE_PACKET) {
			if (buf)
				_gnutls_buffer_init(buf);
			return 0;
		}

		return gnutls_assert_val_fatal(ret);
	}

	session->internals.last_handshake_in = hsk.htype;

	ret = call_hook_func(session, hsk.htype, GNUTLS_HOOK_PRE, 1, hsk.data.data, hsk.data.length);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = handshake_hash_add_recvd(session, hsk.htype,
				       hsk.header, hsk.header_size,
				       hsk.data.data,
				       hsk.data.length);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	switch (hsk.htype) {
	case GNUTLS_HANDSHAKE_CLIENT_HELLO_V2:
	case GNUTLS_HANDSHAKE_CLIENT_HELLO:
	case GNUTLS_HANDSHAKE_SERVER_HELLO:
		if (hsk.htype == GNUTLS_HANDSHAKE_CLIENT_HELLO_V2)
			ret =
			    _gnutls_read_client_hello_v2(session,
							 hsk.data.data,
							 hsk.data.length);
		else
			ret =
			    recv_hello(session, hsk.data.data,
					hsk.data.length);

		if (ret < 0) {
			gnutls_assert();
			goto cleanup;
		}

		break;
	case GNUTLS_HANDSHAKE_HELLO_VERIFY_REQUEST:
		ret =
		    recv_hello_verify_request(session,
					      hsk.data.data,
					      hsk.data.length);
		if (ret < 0) {
			gnutls_assert();
			goto cleanup;
		} else {
			/* Signal our caller we have received a verification cookie
			   and ClientHello needs to be sent again. */
			ret = 1;
		}

		break;
	case GNUTLS_HANDSHAKE_SERVER_HELLO_DONE:
		if (hsk.data.length == 0)
			ret = 0;
		else {
			gnutls_assert();
			ret = GNUTLS_E_UNEXPECTED_PACKET_LENGTH;
			goto cleanup;
		}
		break;
	case GNUTLS_HANDSHAKE_CERTIFICATE_PKT:
	case GNUTLS_HANDSHAKE_CERTIFICATE_STATUS:
	case GNUTLS_HANDSHAKE_FINISHED:
	case GNUTLS_HANDSHAKE_SERVER_KEY_EXCHANGE:
	case GNUTLS_HANDSHAKE_CLIENT_KEY_EXCHANGE:
	case GNUTLS_HANDSHAKE_CERTIFICATE_REQUEST:
	case GNUTLS_HANDSHAKE_CERTIFICATE_VERIFY:
	case GNUTLS_HANDSHAKE_SUPPLEMENTAL:
	case GNUTLS_HANDSHAKE_NEW_SESSION_TICKET:
		ret = hsk.data.length;
		break;
	default:
		gnutls_assert();
		/* we shouldn't actually arrive here in any case .
		 * unexpected messages should be catched after _gnutls_handshake_io_recv_int()
		 */
		ret = GNUTLS_E_UNEXPECTED_HANDSHAKE_PACKET;
		goto cleanup;
	}

	ret2 = call_hook_func(session, hsk.htype, GNUTLS_HOOK_POST, 1, hsk.data.data, hsk.data.length);
	if (ret2 < 0) {
		ret = ret2;
		gnutls_assert();
		goto cleanup;
	}

	if (buf) {
		*buf = hsk.data;
		return ret;
	}

      cleanup:
	_gnutls_handshake_buffer_clear(&hsk);
	return ret;
}