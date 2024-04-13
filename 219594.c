set_client_comp_method(gnutls_session_t session,
			       uint8_t comp_method)
{
	int comp_methods_num;
	uint8_t compression_methods[MAX_ALGOS];
	int id = _gnutls_compression_get_id(comp_method);
	int i;

	_gnutls_handshake_log
	    ("HSK[%p]: Selected compression method: %s (%d)\n", session,
	     gnutls_compression_get_name(id), (int) comp_method);

	comp_methods_num = _gnutls_supported_compression_methods(session,
								 compression_methods,
								 MAX_ALGOS);
	if (comp_methods_num < 0) {
		gnutls_assert();
		return comp_methods_num;
	}

	for (i = 0; i < comp_methods_num; i++) {
		if (compression_methods[i] == comp_method) {
			comp_methods_num = 0;
			break;
		}
	}

	if (comp_methods_num != 0) {
		gnutls_assert();
		return GNUTLS_E_UNKNOWN_COMPRESSION_ALGORITHM;
	}

	session->security_parameters.compression_method = id;
	_gnutls_epoch_set_compression(session, EPOCH_NEXT, id);

	return 0;
}