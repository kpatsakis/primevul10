server_select_comp_method(gnutls_session_t session,
			  uint8_t * data, int datalen)
{
	int x, i, j;
	uint8_t comps[MAX_ALGOS];

	x = _gnutls_supported_compression_methods(session, comps,
						  MAX_ALGOS);
	if (x < 0) {
		gnutls_assert();
		return x;
	}

	if (session->internals.priorities.server_precedence == 0) {
		for (j = 0; j < datalen; j++) {
			for (i = 0; i < x; i++) {
				if (comps[i] == data[j]) {
					gnutls_compression_method_t method
					    =
					    _gnutls_compression_get_id
					    (comps[i]);

					_gnutls_epoch_set_compression
					    (session, EPOCH_NEXT, method);
					session->security_parameters.
					    compression_method = method;

					_gnutls_handshake_log
					    ("HSK[%p]: Selected Compression Method: %s\n",
					     session,
					     gnutls_compression_get_name
					     (method));
					return 0;
				}
			}
		}
	} else {
		for (i = 0; i < x; i++) {
			for (j = 0; j < datalen; j++) {
				if (comps[i] == data[j]) {
					gnutls_compression_method_t method
					    =
					    _gnutls_compression_get_id
					    (comps[i]);

					_gnutls_epoch_set_compression
					    (session, EPOCH_NEXT, method);
					session->security_parameters.
					    compression_method = method;

					_gnutls_handshake_log
					    ("HSK[%p]: Selected Compression Method: %s\n",
					     session,
					     gnutls_compression_get_name
					     (method));
					return 0;
				}
			}
		}
	}

	/* we were not able to find a compatible compression
	 * algorithm
	 */
	gnutls_assert();
	return GNUTLS_E_UNKNOWN_COMPRESSION_ALGORITHM;

}