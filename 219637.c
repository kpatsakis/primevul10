gnutls_priority_set(gnutls_session_t session, gnutls_priority_t priority)
{
	if (priority == NULL) {
		gnutls_assert();
		return GNUTLS_E_NO_CIPHER_SUITES;
	}

	memcpy(&session->internals.priorities, priority,
	       sizeof(struct gnutls_priority_st));

	/* set the current version to the first in the chain.
	 * This will be overridden later.
	 */
	if (session->internals.priorities.protocol.algorithms > 0) {
		if (_gnutls_set_current_version(session,
					    session->internals.priorities.
					    protocol.priority[0]) < 0) {
			return gnutls_assert_val(GNUTLS_E_UNSUPPORTED_VERSION_PACKET);
		}
	}

	if (priority->no_tickets != 0) {
		/* when PFS is explicitly requested, disable session tickets */
		_gnutls_ext_unset_session_data(session, GNUTLS_EXTENSION_SESSION_TICKET);
	}

	if (session->internals.priorities.protocol.algorithms == 0 ||
	    session->internals.priorities.cipher.algorithms == 0 ||
	    session->internals.priorities.mac.algorithms == 0 ||
	    session->internals.priorities.kx.algorithms == 0 ||
	    session->internals.priorities.compression.algorithms == 0)
		return gnutls_assert_val(GNUTLS_E_NO_PRIORITIES_WERE_SET);

	return 0;
}