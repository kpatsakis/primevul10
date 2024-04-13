static uint32_t get_ipaddr(char *host) {
	struct hostent *hp;

	if (good_ipaddr(host) == 0) {
		return ipstr2long(host);
	} else if ((hp = gethostbyname(host)) == (struct hostent *)NULL) {
		return (uint32_t)0;
	}

	return ntohl(*(uint32_t *)hp->h_addr);
}