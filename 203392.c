static int acl_parse_ver(const char **text, struct acl_pattern *pattern, int *opaque)
{
	pattern->ptr.str = strdup(*text);
	if (!pattern->ptr.str)
		return 0;
	pattern->len = strlen(*text);
	return 1;
}