size_t rtattr_strlcpy(char *dest, const struct rtattr *rta, size_t size)
{
	size_t ret = RTA_PAYLOAD(rta);
	char *src = RTA_DATA(rta);

	if (ret > 0 && src[ret - 1] == '\0')
		ret--;
	if (size > 0) {
		size_t len = (ret >= size) ? size - 1 : ret;
		memset(dest, 0, size);
		memcpy(dest, src, len);
	}
	return ret;
}