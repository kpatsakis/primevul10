imap_parser_get_last_literal_size(struct imap_parser *parser,
				  ARRAY_TYPE(imap_arg_list) **list_r)
{
	ARRAY_TYPE(imap_arg_list) *list;
	struct imap_arg *args;
	unsigned int count;

	list = &parser->root_list;
	args = array_get_modifiable(&parser->root_list, &count);
	i_assert(count > 1 && args[count-1].type == IMAP_ARG_EOL);
	count--;

	while (args[count-1].type != IMAP_ARG_LITERAL_SIZE &&
	       args[count-1].type != IMAP_ARG_LITERAL_SIZE_NONSYNC) {
		if (args[count-1].type != IMAP_ARG_LIST)
			return NULL;

		/* maybe the list ends with literal size */
		list = &args[count-1]._data.list;
		args = array_get_modifiable(list, &count);
		if (count == 0)
			return NULL;
	}

	*list_r = list;
	return &args[count-1];
}