imap_write_envelope_list(const struct imap_arg *args, string_t *str,
	bool toplevel)
{
	const struct imap_arg *children;

	/* don't do any typechecking, just write it out */
	while (!IMAP_ARG_IS_EOL(args)) {
		bool list = FALSE;

		if (!str_append_nstring(str, args)) {
			if (!imap_arg_get_list(args, &children)) {
				/* everything is either nstring or list */
				i_unreached();
			}

			str_append_c(str, '(');
			imap_write_envelope_list(children, str, FALSE);
			str_append_c(str, ')');

			list = TRUE;
		}
		args++;

		if ((toplevel || !list) && !IMAP_ARG_IS_EOL(args))
			str_append_c(str, ' ');
	}
}