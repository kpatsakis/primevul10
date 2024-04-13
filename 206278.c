imap_bodystructure_parse_args(const struct imap_arg *args, pool_t pool,
			      struct message_part **_part,
			      const char **error_r)
{
	struct message_part *part = *_part, *child_part;;
	struct message_part **child_part_p;
	struct message_part_data *data;
	const struct imap_arg *list_args;
	const char *value, *content_type, *subtype, *error;
	bool multipart, text, message_rfc822, parsing_tree, has_lines;
	unsigned int lines;
	uoff_t vsize;

	if (part != NULL) {
		/* parsing with pre-existing message_part tree */
		parsing_tree = FALSE;
	} else {
		/* parsing message_part tree from BODYSTRUCTURE as well */
		part = *_part = p_new(pool, struct message_part, 1);
		parsing_tree = TRUE;
	}
	part->data = data = p_new(pool, struct message_part_data, 1);

	multipart = FALSE;
	if (!parsing_tree) {
		if ((part->flags & MESSAGE_PART_FLAG_MULTIPART) != 0 &&
			part->children == NULL) {
			struct message_part_data dummy_part_data = {
				.content_type = "text",
				.content_subtype = "plain",
				.content_transfer_encoding = "7bit"
			};
			struct message_part dummy_part = {
				.parent = part,
				.data = &dummy_part_data,
				.flags = MESSAGE_PART_FLAG_TEXT
			};
			struct message_part *dummy_partp = &dummy_part;

			/* no parts in multipart message,
			   that's not allowed. expect a single
			   0-length text/plain structure */
			if (args->type != IMAP_ARG_LIST ||
				(args+1)->type == IMAP_ARG_LIST) {
				*error_r = "message_part hierarchy "
					"doesn't match BODYSTRUCTURE";
				return -1;
			}

			list_args = imap_arg_as_list(args);
			if (imap_bodystructure_parse_args(list_args, pool,
								&dummy_partp, error_r) < 0)
				return -1;
			child_part = NULL;

			multipart = TRUE;
			args++;

		} else {
			child_part = part->children;
			while (args->type == IMAP_ARG_LIST) {
				if ((part->flags & MESSAGE_PART_FLAG_MULTIPART) == 0 ||
						child_part == NULL) {
					*error_r = "message_part hierarchy "
						"doesn't match BODYSTRUCTURE";
					return -1;
				}

				list_args = imap_arg_as_list(args);
				if (imap_bodystructure_parse_args(list_args, pool,
									&child_part, error_r) < 0)
					return -1;
				child_part = child_part->next;

				multipart = TRUE;
				args++;
			}
		}
		if (multipart) {
			if (child_part != NULL) {
				*error_r = "message_part hierarchy "
					"doesn't match BODYSTRUCTURE";
				return -1;
			}
		} else 	if ((part->flags & MESSAGE_PART_FLAG_MULTIPART) != 0) {
			*error_r = "message_part multipart flag "
				"doesn't match BODYSTRUCTURE";
			return -1;
		}
	} else {
		child_part_p = &part->children;
		while (args->type == IMAP_ARG_LIST) {
			list_args = imap_arg_as_list(args);
			if (imap_bodystructure_parse_args(list_args, pool,
								child_part_p, error_r) < 0)
				return -1;
			(*child_part_p)->parent = part;
			child_part_p = &(*child_part_p)->next;

			multipart = TRUE;
			args++;
		}
		if (multipart) {
			part->flags |= MESSAGE_PART_FLAG_MULTIPART;
		}
	}

	if (multipart) {
		data->content_type = "multipart";
		if (!imap_arg_get_nstring(args++, &data->content_subtype)) {
			*error_r = "Invalid multipart content-type";
			return -1;
		}
		data->content_subtype = p_strdup(pool, data->content_subtype);
		if (args->type == IMAP_ARG_EOL)
			return 0;
		if (imap_bodystructure_params_parse(args++, pool,
			&data->content_type_params,
			&data->content_type_params_count) < 0) {
			*error_r = "Invalid content params";
			return -1;
		}
		return imap_bodystructure_parse_args_common
			(part, pool, args, error_r);
	}

	/* "content type" "subtype" */
	if (!imap_arg_get_astring(&args[0], &content_type) ||
	    !imap_arg_get_astring(&args[1], &subtype)) {
		*error_r = "Invalid content-type";
		return -1;
	}
	data->content_type = p_strdup(pool, content_type);
	data->content_subtype = p_strdup(pool, subtype);
	args += 2;

	text = strcasecmp(content_type, "text") == 0;
	message_rfc822 = strcasecmp(content_type, "message") == 0 &&
		strcasecmp(subtype, "rfc822") == 0;

	if (!parsing_tree) {
#if 0
		/* Disabled for now. Earlier Dovecot versions handled broken
		   Content-Type headers by writing them as "text" "plain" to
		   BODYSTRUCTURE reply, but the message_part didn't have
		   MESSAGE_PART_FLAG_TEXT. */
		if (text != ((part->flags & MESSAGE_PART_FLAG_TEXT) != 0)) {
			*error_r = "message_part text flag "
				"doesn't match BODYSTRUCTURE";
			return -1;
		}
#endif
		if (message_rfc822 !=
			((part->flags & MESSAGE_PART_FLAG_MESSAGE_RFC822) != 0)) {
			*error_r = "message_part message/rfc822 flag "
				"doesn't match BODYSTRUCTURE";
			return -1;
		}
	} else {
		if (text)
			part->flags |= MESSAGE_PART_FLAG_TEXT;
		if (message_rfc822)
			part->flags |= MESSAGE_PART_FLAG_MESSAGE_RFC822;
	}

	/* ("content type param key" "value" ...) | NIL */
	if (imap_bodystructure_params_parse(args++, pool,
		&data->content_type_params,
		&data->content_type_params_count) < 0) {
		*error_r = "Invalid content params";
		return -1;
	}

	/* "content id" "content description" "transfer encoding" size */
	if (!imap_arg_get_nstring(args++, &data->content_id)) {
		*error_r = "Invalid content-id";
		return -1;
	}
	if (!imap_arg_get_nstring(args++, &data->content_description)) {
		*error_r = "Invalid content-description";
		return -1;
	}
	if (!imap_arg_get_nstring(args++, &data->content_transfer_encoding)) {
		*error_r = "Invalid content-transfer-encoding";
		return -1;
	}
	data->content_id = p_strdup(pool, data->content_id);
	data->content_description = p_strdup(pool, data->content_description);
	data->content_transfer_encoding =
		p_strdup(pool, data->content_transfer_encoding);
	if (!imap_arg_get_atom(args++, &value) ||
	    str_to_uoff(value, &vsize) < 0) {
		*error_r = "Invalid size field";
		return -1;
	}
	if (!parsing_tree) {
		if (vsize != part->body_size.virtual_size) {
			*error_r = "message_part virtual_size doesn't match "
				"size in BODYSTRUCTURE";
			return -1;
		}
	} else {
		part->body_size.virtual_size = vsize;
	}

	if (text) {
		/* text/xxx - text lines */
		if (!imap_arg_get_atom(args++, &value) ||
		    str_to_uint(value, &lines) < 0) {
			*error_r = "Invalid lines field";
			return -1;
		}
		i_assert(part->children == NULL);
		has_lines = TRUE;
	} else if (message_rfc822) {
		/* message/rfc822 - envelope + bodystructure + text lines */

		if (!parsing_tree) {
			i_assert(part->children != NULL &&
				 part->children->next == NULL);
		}

		if (!imap_arg_get_list(&args[1], &list_args)) {
			*error_r = "Child bodystructure list expected";
			return -1;
		}
		if (imap_bodystructure_parse_args
			(list_args, pool, &part->children, error_r) < 0)
			return -1;
		if (parsing_tree) {
			i_assert(part->children != NULL &&
				 part->children->next == NULL);
			part->children->parent = part;
		}

		if (!imap_arg_get_list(&args[0], &list_args)) {
			*error_r = "Envelope list expected";
			return -1;
		}
		if (!imap_envelope_parse_args(list_args, pool,
			&part->children->data->envelope, &error)) {
			*error_r = t_strdup_printf
				("Invalid envelope list: %s", error);
			return -1;
		}
		args += 2;
		if (!imap_arg_get_atom(args++, &value) ||
		    str_to_uint(value, &lines) < 0) {
			*error_r = "Invalid lines field";
			return -1;
		}
		has_lines = TRUE;
	} else {
		i_assert(part->children == NULL);
		lines = 0;
		has_lines = FALSE;
	}
	if (!parsing_tree) {
		if (has_lines && lines != part->body_size.lines) {
			*error_r = "message_part lines "
				"doesn't match lines in BODYSTRUCTURE";
			return -1;
		}
	} else {
		part->body_size.lines = lines;
	}
	if (args->type == IMAP_ARG_EOL)
		return 0;
	if (!imap_arg_get_nstring(args++, &data->content_md5)) {
		*error_r = "Invalid content-md5";
		return -1;
	}
	data->content_md5 = p_strdup(pool, data->content_md5);
	return imap_bodystructure_parse_args_common
		(part, pool, args, error_r);
}