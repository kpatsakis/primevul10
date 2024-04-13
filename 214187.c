static int read_var_sized(struct archive_read* a, size_t* pvalue,
    size_t* pvalue_len)
{
	uint64_t v;
	uint64_t v_size = 0;

	const int ret = pvalue_len ? read_var(a, &v, &v_size)
				   : read_var(a, &v, NULL);

	if(ret == 1 && pvalue) {
		*pvalue = (size_t) v;
	}

	if(pvalue_len) {
		/* Possible data truncation should be safe. */
		*pvalue_len = (size_t) v_size;
	}

	return ret;
}