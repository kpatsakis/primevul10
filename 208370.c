static void get_accounting_vector(AUTH_HDR *request, radius_server_t *server)
{
	MD5_CTX my_md5;
	int secretlen = strlen(server->secret);
	int len = ntohs(request->length);

	memset(request->vector, 0, AUTH_VECTOR_LEN);
	MD5Init(&my_md5);
	memcpy(((char *)request) + len, server->secret, secretlen);

	MD5Update(&my_md5, (unsigned char *)request, len + secretlen);
	MD5Final(request->vector, &my_md5);			/* set the final vector */
}