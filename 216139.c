R_API char *r_socket_http_get(const char *url, int *code, int *rlen) {
	return socket_http_get_recursive (url, code, rlen, SOCKET_HTTP_MAX_REDIRECTS);
}