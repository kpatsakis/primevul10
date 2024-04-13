imap_parser_strdup(struct imap_parser *parser,
		   const void *data, size_t len)
{
	char *ret;

	ret = p_malloc(parser->pool, len + 1);
	memcpy(ret, data, len);
	return ret;
}