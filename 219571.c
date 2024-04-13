static int handshake_client(gnutls_session_t session)
{
	int ret = 0;

#ifdef HANDSHAKE_DEBUG
	char buf[64];

	if (session->internals.resumed_security_parameters.
	    session_id_size > 0)
		_gnutls_handshake_log("HSK[%p]: Ask to resume: %s\n",
				      session,
				      _gnutls_bin2hex(session->internals.
						      resumed_security_parameters.
						      session_id,
						      session->internals.
						      resumed_security_parameters.
						      session_id_size, buf,
						      sizeof(buf), NULL));
#endif

	switch (STATE) {
	case STATE0:
	case STATE1:
		ret = send_hello(session, AGAIN(STATE1));
		STATE = STATE1;
		IMED_RET("send hello", ret, 1);

	case STATE2:
		if (IS_DTLS(session)) {
			ret =
			    _gnutls_recv_handshake(session,
						   GNUTLS_HANDSHAKE_HELLO_VERIFY_REQUEST,
						   1, NULL);
			STATE = STATE2;
			IMED_RET("recv hello verify", ret, 1);

			if (ret == 1) {
				STATE = STATE0;
				return 1;
			}
		}
	case STATE3:
		/* receive the server hello */
		ret =
		    _gnutls_recv_handshake(session,
					   GNUTLS_HANDSHAKE_SERVER_HELLO,
					   0, NULL);
		STATE = STATE3;
		IMED_RET("recv hello", ret, 1);

	case STATE4:
		ret = _gnutls_ext_sr_verify(session);
		STATE = STATE4;
		IMED_RET("recv hello", ret, 0);

	case STATE5:
		if (session->security_parameters.do_recv_supplemental) {
			ret = _gnutls_recv_supplemental(session);
			STATE = STATE5;
			IMED_RET("recv supplemental", ret, 1);
		}

	case STATE6:
		/* RECV CERTIFICATE */
		if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
			ret = _gnutls_recv_server_certificate(session);
		STATE = STATE6;
		IMED_RET("recv server certificate", ret, 1);

	case STATE7:
#ifdef ENABLE_OCSP
		/* RECV CERTIFICATE STATUS */
		if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
			ret =
			    _gnutls_recv_server_certificate_status
			    (session);
		STATE = STATE7;
		IMED_RET("recv server certificate", ret, 1);
#endif
	case STATE8:
		ret = run_verify_callback(session, GNUTLS_CLIENT);
		STATE = STATE8;
		if (ret < 0)
			return gnutls_assert_val(ret);
	case STATE9:
		/* receive the server key exchange */
		if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
			ret = _gnutls_recv_server_kx_message(session);
		STATE = STATE9;
		IMED_RET("recv server kx message", ret, 1);

	case STATE10:
		/* receive the server certificate request - if any 
		 */

		if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
			ret = _gnutls_recv_server_crt_request(session);
		STATE = STATE10;
		IMED_RET("recv server certificate request message", ret,
			 1);

	case STATE11:
		/* receive the server hello done */
		if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
			ret =
			    _gnutls_recv_handshake(session,
						   GNUTLS_HANDSHAKE_SERVER_HELLO_DONE,
						   0, NULL);
		STATE = STATE11;
		IMED_RET("recv server hello done", ret, 1);

	case STATE12:
		if (session->security_parameters.do_send_supplemental) {
			ret =
			    _gnutls_send_supplemental(session,
						      AGAIN(STATE12));
			STATE = STATE12;
			IMED_RET("send supplemental", ret, 0);
		}

	case STATE13:
		/* send our certificate - if any and if requested
		 */
		if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
			ret =
			    _gnutls_send_client_certificate(session,
							    AGAIN
							    (STATE13));
		STATE = STATE13;
		IMED_RET("send client certificate", ret, 0);

	case STATE14:
		if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
			ret =
			    _gnutls_send_client_kx_message(session,
							   AGAIN(STATE14));
		STATE = STATE14;
		IMED_RET("send client kx", ret, 0);

	case STATE15:
		/* send client certificate verify */
		if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
			ret =
			    _gnutls_send_client_certificate_verify(session,
								   AGAIN
								   (STATE15));
		STATE = STATE15;
		IMED_RET("send client certificate verify", ret, 1);

	case STATE16:
		STATE = STATE16;
		if (session->internals.resumed == RESUME_FALSE) {
			ret = send_handshake_final(session, TRUE);
			IMED_RET("send handshake final 2", ret, 1);
#ifdef ENABLE_SESSION_TICKETS
		} else {
			ret = _gnutls_recv_new_session_ticket(session);
			IMED_RET("recv handshake new session ticket", ret,
				 1);
#endif
		}

	case STATE17:
		STATE = STATE17;
		if (session->internals.resumed == RESUME_FALSE) {
#ifdef ENABLE_SESSION_TICKETS
			ret = _gnutls_recv_new_session_ticket(session);
			IMED_RET("recv handshake new session ticket", ret,
				 1);
#endif
		} else {
			ret = recv_handshake_final(session, TRUE);
			IMED_RET("recv handshake final", ret, 1);
		}

	case STATE18:
		STATE = STATE18;
		if (session->internals.resumed == RESUME_FALSE) {
			ret = recv_handshake_final(session, FALSE);
			IMED_RET("recv handshake final 2", ret, 1);
		} else {
			ret = send_handshake_final(session, FALSE);
			IMED_RET("send handshake final", ret, 1);
		}

		STATE = STATE0;
	default:
		break;
	}

	return 0;
}