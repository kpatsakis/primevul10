static char *socket_http_get_recursive(const char *url, int *code, int *rlen, ut32 redirections) {
	if (code) {
		*code = 0;
	}
	if (rlen) {
		*rlen = 0;
	}
	char *curl_env = r_sys_getenv ("R2_CURL");
	if (!R_STR_ISEMPTY (curl_env) && atoi (curl_env)) {
		int len;
		char *escaped_url = r_str_escape_sh (url);
		char *command = r_str_newf ("curl -sfL -o - \"%s\"", escaped_url);
		char *res = r_sys_cmd_str (command, NULL, &len);
		free (escaped_url);
		free (command);
		free (curl_env);
		if (!res) {
			return NULL;
		}
		if (res) {
			if (code) {
				*code = 200;
			}
			if (rlen) {
				*rlen = len;
			}
		}
		return res;
	}
	free (curl_env);
#if __WINDOWS__
	return http_get_w32 (url, code, rlen);
#else
	RSocket *s;
	int ssl = r_str_startswith (url, "https://");
#if !HAVE_LIB_SSL
	if (ssl) {
		eprintf ("Tried to get '%s', but SSL support is disabled, set R2_CURL=1 to use curl\n", url);
		return NULL;
	}
#endif
	char *response, *host, *path, *port = "80";
	char *uri = strdup (url);
	if (!uri) {
		return NULL;
	}
	host = strstr (uri, "://");
	if (!host) {
		free (uri);
		eprintf ("r_socket_http_get: Invalid URI");
		return NULL;
	}
	host += 3;
	port = strchr (host, ':');
	if (!port) {
		port = ssl? "443": "80";
		path = host;
	} else {
		*port++ = 0;
		path = port;
	}
	path = strchr (path, '/');
	if (!path) {
		path = "";
	} else {
		*path++ = 0;
	}
	s = r_socket_new (ssl);
	if (!s) {
		eprintf ("r_socket_http_get: Cannot create socket\n");
		free (uri);
		return NULL;
	}
	if (r_socket_connect_tcp (s, host, port, 0)) {
		r_socket_printf (s,
				"GET /%s HTTP/1.1\r\n"
				"User-Agent: radare2 "R2_VERSION"\r\n"
				"Accept: */*\r\n"
				"Host: %s:%s\r\n"
				"\r\n", path, host, port);
		response = socket_http_answer (s, code, rlen, redirections);
	} else {
		eprintf ("Cannot connect to %s:%s\n", host, port);
		response = NULL;
	}
	free (uri);
	r_socket_free (s);
	return response;
#endif
}