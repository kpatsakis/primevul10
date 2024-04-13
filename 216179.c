static void trimbits(char *b) {
	const int len = strlen (b);
	char *one = strchr (b, '1');
	int pos = one ? (int)(size_t)(one - b) : len - 1;
	pos = (pos / 8) * 8;
	memmove (b, b + pos, len - pos + 1);
}