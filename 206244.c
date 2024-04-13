static bool imap_is_valid_tag(const char *tag)
{
	for (; *tag != '\0'; tag++) {
		switch (*tag) {
		case '+':
		/* atom-specials: */
		case '(':
		case ')':
		case '{':
		case '/':
		case ' ':
		/* list-wildcards: */
		case '%':
		case '*':
		/* quoted-specials: */
		case '"':
		case '\\':
			return FALSE;
		default:
			if (*tag < ' ') /* CTL */
				return FALSE;
			break;
		}
	}
	return TRUE;
}