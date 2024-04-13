imap_bodystructure_parse_args_common(struct message_part *part,
				     pool_t pool, const struct imap_arg *args,
				     const char **error_r)
{
	struct message_part_data *data = part->data;
	const struct imap_arg *list_args;

	if (args->type == IMAP_ARG_EOL)
		return 0;
	if (args->type == IMAP_ARG_NIL)
		args++;
	else if (!imap_arg_get_list(args, &list_args)) {
		*error_r = "Invalid content-disposition list";
		return -1;
	} else {
		if (!imap_arg_get_nstring
			(list_args++, &data->content_disposition)) {
			*error_r = "Invalid content-disposition";
			return -1;
		}
		data->content_disposition = p_strdup(pool, data->content_disposition);
		if (imap_bodystructure_params_parse(list_args, pool,
			&data->content_disposition_params,
			&data->content_disposition_params_count) < 0) {
			*error_r = "Invalid content-disposition params";
			return -1;
		}
		args++;
	}
	if (args->type == IMAP_ARG_EOL)
		return 0;
	if (imap_bodystructure_strlist_parse
		(args++, pool, &data->content_language) < 0) {
		*error_r = "Invalid content-language";
		return -1;
	}
	if (args->type == IMAP_ARG_EOL)
		return 0;
	if (!imap_arg_get_nstring
		(args++, &data->content_location)) {
		*error_r = "Invalid content-location";
		return -1;
	}
	data->content_location = p_strdup(pool, data->content_location);
	return 0;
}