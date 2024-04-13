mygetpwuid_r(uid_t uid, struct passwd *pwd, struct passwd **result)
{
	int rv;
	char *buf;
	static long pwsz = 0;
	size_t buflen;

	*result = NULL;

	if (pwsz == 0)
		pwsz = sysconf(_SC_GETPW_R_SIZE_MAX);

	buflen = pwsz > 0 ? pwsz : 1024;

	buf = malloc(buflen);
	if (buf == NULL)
		return errno;

	while ((rv = getpwuid_r(uid, pwd, buf, buflen, result)) == ERANGE) {
		size_t newsz;
		newsz = buflen * 2;
		if (newsz < buflen)
			return rv;
		buflen = newsz;
		buf = realloc(buf, buflen);
		if (buf == NULL)
			return errno;
	}

	return rv;
}