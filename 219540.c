set_client_ciphersuite(gnutls_session_t session, uint8_t suite[2])
{
	uint8_t z;
	uint8_t cipher_suites[MAX_CIPHERSUITE_SIZE];
	int cipher_suite_size;
	int i;

	z = 1;
	cipher_suite_size =
	    _gnutls_supported_ciphersuites(session, cipher_suites,
					   sizeof(cipher_suites));
	if (cipher_suite_size < 0) {
		gnutls_assert();
		return cipher_suite_size;
	}

	for (i = 0; i < cipher_suite_size; i += 2) {
		if (memcmp(&cipher_suites[i], suite, 2) == 0) {
			z = 0;
			break;
		}
	}

	if (z != 0) {
		gnutls_assert();
		_gnutls_handshake_log
		    ("HSK[%p]: unsupported cipher suite %.2X.%.2X\n",
		     session, (unsigned int) suite[0],
		     (unsigned int) suite[1]);
		return GNUTLS_E_UNKNOWN_CIPHER_SUITE;
	}

	memcpy(session->security_parameters.cipher_suite, suite, 2);
	_gnutls_epoch_set_cipher_suite(session, EPOCH_NEXT,
				       session->security_parameters.
				       cipher_suite);

	_gnutls_handshake_log("HSK[%p]: Selected cipher suite: %s\n",
			      session,
			      _gnutls_cipher_suite_get_name
			      (session->security_parameters.cipher_suite));


	/* check if the credentials (username, public key etc.) are ok.
	 * Actually checks if they exist.
	 */
	if (!session->internals.premaster_set &&
	    _gnutls_get_kx_cred
	    (session,
	     _gnutls_cipher_suite_get_kx_algo
	     (session->security_parameters.cipher_suite)) == NULL) {
		gnutls_assert();
		return GNUTLS_E_INSUFFICIENT_CREDENTIALS;
	}


	/* set the mod_auth_st to the appropriate struct
	 * according to the KX algorithm. This is needed since all the
	 * handshake functions are read from there;
	 */
	session->internals.auth_struct =
	    _gnutls_kx_auth_struct(_gnutls_cipher_suite_get_kx_algo
				   (session->security_parameters.
				    cipher_suite));

	if (session->internals.auth_struct == NULL) {

		_gnutls_handshake_log
		    ("HSK[%p]: Cannot find the appropriate handler for the KX algorithm\n",
		     session);
		gnutls_assert();
		return GNUTLS_E_INTERNAL_ERROR;
	}


	return 0;
}