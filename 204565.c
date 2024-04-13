static SSL_METHOD *dtls1_get_server_method(int ver)
	{
	if (ver == DTLS1_VERSION)
		return(DTLSv1_server_method());
	else
		return(NULL);
	}