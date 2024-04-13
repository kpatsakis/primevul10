static int run_verify_callback(gnutls_session_t session, unsigned int side)
{
	gnutls_certificate_credentials_t cred;
	int ret, type;

	cred =
	    (gnutls_certificate_credentials_t) _gnutls_get_cred(session,
								GNUTLS_CRD_CERTIFICATE);

	if (side == GNUTLS_CLIENT)
		type = gnutls_auth_server_get_type(session);
	else
		type = gnutls_auth_client_get_type(session);

	if (type != GNUTLS_CRD_CERTIFICATE)
		return 0;

	if (cred != NULL && cred->verify_callback != NULL &&
	    (session->security_parameters.entity == GNUTLS_CLIENT ||
	     session->internals.send_cert_req != GNUTLS_CERT_IGNORE)) {
		ret = cred->verify_callback(session);
		if (ret < -1)
			return ret;
		else if (ret != 0)
			return GNUTLS_E_CERTIFICATE_ERROR;
	}

	return 0;
}