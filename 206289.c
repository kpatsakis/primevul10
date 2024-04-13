imap_bodystructure_strlist_parse(const struct imap_arg *arg,
	pool_t pool, const char *const **list_r)
{
	const char *item, **list;
	const struct imap_arg *list_args;
	unsigned int list_count, i;

	if (arg->type == IMAP_ARG_NIL) {
		*list_r = NULL;
		return 0;
	}
	if (imap_arg_get_nstring(arg, &item)) {
		list = p_new(pool, const char *, 2);
		list[0] = p_strdup(pool, item);
	} else {
		if (!imap_arg_get_list_full(arg, &list_args, &list_count))
			return -1;

		list = p_new(pool, const char *, list_count+1);
		for (i = 0; i < list_count; i++) {
			if (!imap_arg_get_nstring(&list_args[i], &item))
				return -1;
			list[i] = p_strdup(pool, item);
		}
	}
	*list_r = list;
	return 0;
}