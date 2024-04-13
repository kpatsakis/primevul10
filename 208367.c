static uint32_t ipstr2long(char *ip_str) {
	char	buf[6];
	char	*ptr;
	int	i;
	int	count;
	uint32_t	ipaddr;
	int	cur_byte;

	ipaddr = (uint32_t)0;

	for(i = 0;i < 4;i++) {
		ptr = buf;
		count = 0;
		*ptr = '\0';

		while(*ip_str != '.' && *ip_str != '\0' && count < 4) {
			if (!isdigit(*ip_str)) {
				return (uint32_t)0;
			}
			*ptr++ = *ip_str++;
			count++;
		}

		if (count >= 4 || count == 0) {
			return (uint32_t)0;
		}

		*ptr = '\0';
		cur_byte = atoi(buf);
		if (cur_byte < 0 || cur_byte > 255) {
			return (uint32_t)0;
		}

		ip_str++;
		ipaddr = ipaddr << 8 | (uint32_t)cur_byte;
	}
	return ipaddr;
}