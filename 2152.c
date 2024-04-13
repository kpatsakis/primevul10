char *get_socket_path(const char *_token)
{
	char *path;
	char *token = xstrdup(_token);

	for (char *c = token; *c; c++) {
		if (*c == '/' || *c == '.')
			*c = '=';
	}

	xasprintf(&path, TMATE_WORKDIR "/sessions/%s", token);
	free(token);
	return path;
}