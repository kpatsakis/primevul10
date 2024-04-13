_gnutls_negotiate_version(gnutls_session_t session,
			  gnutls_protocol_t adv_version)
{
	int ret;

	/* if we do not support that version  */
	if (_gnutls_version_is_supported(session, adv_version) == 0) {
		/* If he requested something we do not support
		 * then we send him the highest we support.
		 */
		ret = _gnutls_version_max(session);
		if (ret == GNUTLS_VERSION_UNKNOWN) {
			/* this check is not really needed.
			 */
			gnutls_assert();
			return GNUTLS_E_UNKNOWN_CIPHER_SUITE;
		}
	} else {
		ret = adv_version;
	}

	if (_gnutls_set_current_version(session, ret) < 0)
		return gnutls_assert_val(GNUTLS_E_UNSUPPORTED_VERSION_PACKET);

	return ret;
}