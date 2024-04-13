chomp (char *str)
{
	size_t len = 0;

	if (str == NULL) return;
	len = strlen(str);
	if ((len > 0) && str[len - 1] == 10) {
		str[len - 1] = 0;
		len--;                                                       
	}
	if ((len > 0) && str[len - 1] == 13)
		str[len - 1] = 0;

	return;
}