static void parse_sec_attr(sc_file_t *file, const u8 *buf, size_t len)
{
	size_t i;
	const int *idx;

	idx = (file->type == SC_FILE_TYPE_DF) ?  df_acl : ef_acl;

	/* acl defaults to 0xFF if unspecified */
	for (i = 0; i < 9; i++)
		if (idx[i] != -1)
			add_acl_entry(file, idx[i], (u8)((i < len) ? buf[i] : 0xFF));
}