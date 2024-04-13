static int resume_copy_required_values(gnutls_session_t session)
{
	int ret;

	/* get the new random values */
	memcpy(session->internals.resumed_security_parameters.
	       server_random, session->security_parameters.server_random,
	       GNUTLS_RANDOM_SIZE);
	memcpy(session->internals.resumed_security_parameters.
	       client_random, session->security_parameters.client_random,
	       GNUTLS_RANDOM_SIZE);

	/* keep the ciphersuite and compression 
	 * That is because the client must see these in our
	 * hello message.
	 */
	memcpy(session->security_parameters.cipher_suite,
	       session->internals.resumed_security_parameters.cipher_suite,
	       2);
	session->security_parameters.compression_method =
	    session->internals.resumed_security_parameters.
	    compression_method;

	ret = _gnutls_epoch_set_cipher_suite(session, EPOCH_NEXT,
					     session->internals.
					     resumed_security_parameters.
					     cipher_suite);
	if (ret < 0)
		return gnutls_assert_val(ret);

	ret = _gnutls_epoch_set_compression(session, EPOCH_NEXT,
					    session->internals.
					    resumed_security_parameters.
					    compression_method);
	if (ret < 0)
		return gnutls_assert_val(ret);

	/* or write_compression_algorithm
	 * they are the same
	 */

	session->security_parameters.entity =
	    session->internals.resumed_security_parameters.entity;

	if (session->internals.resumed_security_parameters.pversion ==
	    NULL)
		return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);

	if (_gnutls_set_current_version(session,
				    session->internals.
				    resumed_security_parameters.pversion->
				    id) < 0)
		return gnutls_assert_val(GNUTLS_E_UNSUPPORTED_VERSION_PACKET);

	session->security_parameters.cert_type =
	    session->internals.resumed_security_parameters.cert_type;

	memcpy(session->security_parameters.session_id,
	       session->internals.resumed_security_parameters.session_id,
	       sizeof(session->security_parameters.session_id));
	session->security_parameters.session_id_size =
	    session->internals.resumed_security_parameters.session_id_size;

	return 0;
}