static int handshake_server(gnutls_session_t session)
{
	int ret = 0;

	switch (STATE) {
	case STATE0:
	case STATE1:
		ret =
		    _gnutls_recv_handshake(session,
					   GNUTLS_HANDSHAKE_CLIENT_HELLO,
					   0, NULL);
		if (ret == GNUTLS_E_INT_RET_0) {
			/* this is triggered by post_client_hello, and instructs the
			 * handshake to proceed but be put on hold */
			ret = GNUTLS_E_INTERRUPTED;
			STATE = STATE2; /* hello already parsed -> move on */
		} else {
			STATE = STATE1;
		}
		IMED_RET("recv hello", ret, 1);

	case STATE2:
		ret = _gnutls_ext_sr_verify(session);
		STATE = STATE2;
		IMED_RET("recv hello", ret, 0);

	case STATE3:
		ret = send_hello(session, AGAIN(STATE3));
		STATE = STATE3;
		IMED_RET("send hello", ret, 1);

	case STATE4:
		if (session->security_parameters.do_send_supplemental) {
			ret =
			    _gnutls_send_supplemental(session,
						      AGAIN(STATE4));
			STATE = STATE4;
			IMED_RET("send supplemental data", ret, 0);
		}

		/* SEND CERTIFICATE + KEYEXCHANGE + CERTIFICATE_REQUEST */
	case STATE5:
		/* NOTE: these should not be send if we are resuming */

		if (session->internals.resumed == RESUME_FALSE)
			ret =
			    _gnutls_send_server_certificate(session,
							    AGAIN(STATE5));
		STATE = STATE5;
		IMED_RET("send server certificate", ret, 0);

	case STATE6:
#ifdef ENABLE_OCSP
		if (session->internals.resumed == RESUME_FALSE)
			ret =
			    _gnutls_send_server_certificate_status(session,
								   AGAIN
								   (STATE6));
		STATE = STATE6;
		IMED_RET("send server certificate status", ret, 0);
#endif
	case STATE7:
		/* send server key exchange (A) */
		if (session->internals.resumed == RESUME_FALSE)
			ret =
			    _gnutls_send_server_kx_message(session,
							   AGAIN(STATE7));
		STATE = STATE7;
		IMED_RET("send server kx", ret, 0);

	case STATE8:
		/* Send certificate request - if requested to */
		if (session->internals.resumed == RESUME_FALSE)
			ret =
			    _gnutls_send_server_crt_request(session,
							    AGAIN(STATE8));
		STATE = STATE8;
		IMED_RET("send server cert request", ret, 0);

	case STATE9:
		/* send the server hello done */
		if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
			ret =
			    _gnutls_send_empty_handshake(session,
							 GNUTLS_HANDSHAKE_SERVER_HELLO_DONE,
							 AGAIN(STATE9));
		STATE = STATE9;
		IMED_RET("send server hello done", ret, 1);

	case STATE10:
		if (session->security_parameters.do_recv_supplemental) {
			ret = _gnutls_recv_supplemental(session);
			STATE = STATE10;
			IMED_RET("recv client supplemental", ret, 1);
		}

		/* RECV CERTIFICATE + KEYEXCHANGE + CERTIFICATE_VERIFY */
	case STATE11:
		/* receive the client certificate message */
		if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
			ret = _gnutls_recv_client_certificate(session);
		STATE = STATE11;
		IMED_RET("recv client certificate", ret, 1);

	case STATE12:
		ret = run_verify_callback(session, GNUTLS_SERVER);
		STATE = STATE12;
		if (ret < 0)
			return gnutls_assert_val(ret);

	case STATE13:
		/* receive the client key exchange message */
		if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
			ret = _gnutls_recv_client_kx_message(session);
		STATE = STATE13;
		IMED_RET("recv client kx", ret, 1);

	case STATE14:
		/* receive the client certificate verify message */
		if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
			ret =
			    _gnutls_recv_client_certificate_verify_message
			    (session);
		STATE = STATE14;
		IMED_RET("recv client certificate verify", ret, 1);

	case STATE15:
		STATE = STATE15;
		if (session->internals.resumed == RESUME_FALSE) {	/* if we are not resuming */
			ret = recv_handshake_final(session, TRUE);
			IMED_RET("recv handshake final", ret, 1);
		} else {
			ret = send_handshake_final(session, TRUE);
			IMED_RET("send handshake final 2", ret, 1);
		}

	case STATE16:
#ifdef ENABLE_SESSION_TICKETS
		ret =
		    _gnutls_send_new_session_ticket(session,
						    AGAIN(STATE16));
		STATE = STATE16;
		IMED_RET("send handshake new session ticket", ret, 0);
#endif
	case STATE17:
		STATE = STATE17;
		if (session->internals.resumed == RESUME_FALSE) {	/* if we are not resuming */
			ret = send_handshake_final(session, FALSE);
			IMED_RET("send handshake final", ret, 1);

			if (session->security_parameters.entity ==
			    GNUTLS_SERVER
			    && session->internals.ticket_sent == 0) {
				/* if no ticket, save session data */
				_gnutls_server_register_current_session
				    (session);
			}
		} else {
			ret = recv_handshake_final(session, FALSE);
			IMED_RET("recv handshake final 2", ret, 1);
		}

		STATE = STATE0;
	default:
		break;
	}


	return 0;
}