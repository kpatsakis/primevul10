imap_bodystructure_params_parse(const struct imap_arg *arg,
	pool_t pool, const struct message_part_param **params_r,
	unsigned int *count_r)
{
	struct message_part_param *params;
	const struct imap_arg *list_args;
	unsigned int list_count, params_count, i;

	if (arg->type == IMAP_ARG_NIL) {
		*params_r = NULL;
		return 0;
	}
	if (!imap_arg_get_list_full(arg, &list_args, &list_count))
		return -1;
	if ((list_count % 2) != 0)
		return -1;

	params_count = list_count/2;
	params = p_new(pool, struct message_part_param, params_count+1);
	for (i = 0; i < params_count; i++) {
		const char *name, *value;

		if (!imap_arg_get_nstring(&list_args[i*2+0], &name))
			return -1;
		if (!imap_arg_get_nstring(&list_args[i*2+1], &value))
			return -1;
		params[i].name = p_strdup(pool, name);
		params[i].value = p_strdup(pool, value);
	}
	*params_r = params;
	*count_r = params_count;
	return 0;
}