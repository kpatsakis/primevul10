static int recv_handshake_final(gnutls_session_t session, int init)
{
	int ret = 0;
	uint8_t ccs[3];
	unsigned int ccs_len = 1;
	unsigned int tleft;
	const version_entry_st *vers;

	ret = handshake_remaining_time(session);
	if (ret < 0)
		return gnutls_assert_val(ret);
	tleft = ret;

	switch (FINAL_STATE) {
	case STATE0:
	case STATE30:
		FINAL_STATE = STATE30;

		/* This is the last flight and peer cannot be sure
		 * we have received it unless we notify him. So we
		 * wait for a message and retransmit if needed. */
		if (IS_DTLS(session) && !_dtls_is_async(session) &&
		    (gnutls_record_check_pending(session) +
		     record_check_unprocessed(session)) == 0) {
			ret = _dtls_wait_and_retransmit(session);
			if (ret < 0)
				return gnutls_assert_val(ret);
		}

		vers = get_version(session);
		if (unlikely(vers == NULL))
			return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);

		if (vers->id == GNUTLS_DTLS0_9)
			ccs_len = 3;

		ret =
		    _gnutls_recv_int(session, GNUTLS_CHANGE_CIPHER_SPEC,
				     -1, NULL, ccs, ccs_len, NULL, tleft);
		if (ret <= 0) {
			ERR("recv ChangeCipherSpec", ret);
			gnutls_assert();
			return (ret<0)?ret:GNUTLS_E_UNEXPECTED_PACKET;
		}

		if (vers->id == GNUTLS_DTLS0_9)
			session->internals.dtls.hsk_read_seq++;

		/* Initialize the connection session (start encryption) - in case of server */
		if (init == TRUE) {
			ret = _gnutls_connection_state_init(session);
			if (ret < 0) {
				gnutls_assert();
				return ret;
			}
		}

		ret = _gnutls_read_connection_state_init(session);
		if (ret < 0) {
			gnutls_assert();
			return ret;
		}

		/* fallthrough */
	case STATE31:
		FINAL_STATE = STATE31;

		if (IS_DTLS(session) && !_dtls_is_async(session) &&
		    (gnutls_record_check_pending(session) +
		     record_check_unprocessed(session)) == 0) {
			ret = _dtls_wait_and_retransmit(session);
			if (ret < 0)
				return gnutls_assert_val(ret);
		}

		ret = _gnutls_recv_finished(session);
		if (ret < 0) {
			ERR("recv finished", ret);
			gnutls_assert();
			return ret;
		}
		FINAL_STATE = STATE0;
	default:
		break;
	}


	return 0;
}