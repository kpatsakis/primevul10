int _gnutls_generate_session_id(uint8_t * session_id, uint8_t * len)
{
	int ret;

	*len = GNUTLS_MAX_SESSION_ID_SIZE;

	ret =
	    _gnutls_rnd(GNUTLS_RND_NONCE, session_id,
			GNUTLS_MAX_SESSION_ID_SIZE);
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}

	return 0;
}