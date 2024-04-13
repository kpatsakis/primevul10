_gnutls_user_hello_func(gnutls_session_t session,
			gnutls_protocol_t adv_version)
{
	int ret, sret = 0;

	if (session->internals.user_hello_func != NULL) {
		ret = session->internals.user_hello_func(session);

		if (ret == GNUTLS_E_AGAIN || ret == GNUTLS_E_INTERRUPTED) {
			gnutls_assert();
			sret = GNUTLS_E_INT_RET_0;
		} else if (ret < 0) {
			gnutls_assert();
			return ret;
		}

		/* Here we need to renegotiate the version since the callee might
		 * have disabled some TLS versions.
		 */
		ret = _gnutls_negotiate_version(session, adv_version);
		if (ret < 0) {
			gnutls_assert();
			return ret;
		}
	}
	return sret;
}