static int send_handshake_final(gnutls_session_t session, int init)
{
	int ret = 0;

	/* Send the CHANGE CIPHER SPEC PACKET */

	switch (FINAL_STATE) {
	case STATE0:
	case STATE1:
		ret = send_change_cipher_spec(session, FAGAIN(STATE1));
		FINAL_STATE = STATE0;

		if (ret < 0) {
			ERR("send ChangeCipherSpec", ret);
			gnutls_assert();
			return ret;
		}
		/* Initialize the connection session (start encryption) - in case of client 
		 */
		if (init == TRUE) {
			ret = _gnutls_connection_state_init(session);
			if (ret < 0) {
				gnutls_assert();
				return ret;
			}
		}

		ret = _gnutls_write_connection_state_init(session);
		if (ret < 0) {
			gnutls_assert();
			return ret;
		}

		/* fallthrough */
	case STATE2:
		/* send the finished message */
		ret = _gnutls_send_finished(session, FAGAIN(STATE2));
		FINAL_STATE = STATE2;
		if (ret < 0) {
			ERR("send Finished", ret);
			gnutls_assert();
			return ret;
		}

		FINAL_STATE = STATE0;
	default:
		break;
	}

	return 0;
}