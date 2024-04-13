imap_write_envelope(const struct imap_arg *args, string_t *str)
{
	imap_write_envelope_list(args, str, TRUE);
}