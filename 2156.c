static char* get_full_hostname(void)
{
	struct addrinfo hints, *info;
	char hostname[1024];
	int gai_result;
	char *ret;

	if (gethostname(hostname, sizeof(hostname)) < 0)
		tmate_fatal("cannot get hostname");
	hostname[1023] = '\0';

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME;

	if ((gai_result = getaddrinfo(hostname, NULL, &hints, &info)) != 0) {
		tmate_info("cannot lookup hostname: %s", gai_strerror(gai_result));
		return xstrdup(hostname);
	}

	ret = xstrdup(info->ai_canonname);

	freeaddrinfo(info);
	return ret;
}