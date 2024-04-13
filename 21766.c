http_istoken(const char **bp, const char *e, const char *token)
{
	int fl = strlen(token);
	const char *b;

	AN(bp);
	AN(e);
	AN(token);

	b = *bp;

	if (b + fl + 2 <= e && *b == '"' &&
	    !memcmp(b + 1, token, fl) && b[fl + 1] == '"') {
		*bp += fl + 2;
		return (1);
	}
	if (b + fl <= e && http_tok_at(b, token, fl) &&
	    (b + fl == e || !vct_istchar(b[fl]))) {
		*bp += fl;
		return (1);
	}
	return (0);
}