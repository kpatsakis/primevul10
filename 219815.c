int h2_parse_cont_len_header(unsigned int *msgf, struct ist *value, unsigned long long *body_len)
{
	char *e, *n;
	unsigned long long cl;
	int not_first = !!(*msgf & H2_MSGF_BODY_CL);
	struct ist word;

	word.ptr = value->ptr - 1; // -1 for next loop's pre-increment
	e = value->ptr + value->len;

	while (++word.ptr < e) {
		/* skip leading delimiter and blanks */
		if (unlikely(HTTP_IS_LWS(*word.ptr)))
			continue;

		/* digits only now */
		for (cl = 0, n = word.ptr; n < e; n++) {
			unsigned int c = *n - '0';
			if (unlikely(c > 9)) {
				/* non-digit */
				if (unlikely(n == word.ptr)) // spaces only
					goto fail;
				break;
			}
			if (unlikely(cl > ULLONG_MAX / 10ULL))
				goto fail; /* multiply overflow */
			cl = cl * 10ULL;
			if (unlikely(cl + c < cl))
				goto fail; /* addition overflow */
			cl = cl + c;
		}

		/* keep a copy of the exact cleaned value */
		word.len = n - word.ptr;

		/* skip trailing LWS till next comma or EOL */
		for (; n < e; n++) {
			if (!HTTP_IS_LWS(*n)) {
				if (unlikely(*n != ','))
					goto fail;
				break;
			}
		}

		/* if duplicate, must be equal */
		if (*msgf & H2_MSGF_BODY_CL && cl != *body_len)
			goto fail;

		/* OK, store this result as the one to be indexed */
		*msgf |= H2_MSGF_BODY_CL;
		*body_len = cl;
		*value = word;
		word.ptr = n;
	}
	/* here we've reached the end with a single value or a series of
	 * identical values, all matching previous series if any. The last
	 * parsed value was sent back into <value>. We just have to decide
	 * if this occurrence has to be indexed (it's the first one) or
	 * silently skipped (it's not the first one)
	 */
	return !not_first;
 fail:
	return -1;
}