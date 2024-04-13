db_dict_field_find(const char *data, void *context,
		   const char **value_r,
		   const char **error_r ATTR_UNUSED)
{
	struct db_dict_value_iter *iter = context;
	struct db_dict_iter_key *key;
	const char *name, *value, *dotname = strchr(data, '.');
	string_t *tmpstr;

	*value_r = NULL;

	if (dotname != NULL)
		data = t_strdup_until(data, dotname++);
	key = db_dict_iter_find_key(iter, data);
	if (key == NULL)
		return 1;

	switch (key->key->parsed_format) {
	case DB_DICT_VALUE_FORMAT_VALUE:
		*value_r = dotname != NULL ? NULL :
			(key->value == NULL ? "" : key->value);
		return 1;
	case DB_DICT_VALUE_FORMAT_JSON:
		if (dotname == NULL)
			return 1;
		db_dict_value_iter_json_init(iter, key->value);
		*value_r = "";
		tmpstr = t_str_new(64);
		while (db_dict_value_iter_json_next(iter, tmpstr, &name, &value)) {
			if (strcmp(name, dotname) == 0) {
				*value_r = t_strdup(value);
				break;
			}
		}
		(void)json_parser_deinit(&iter->json_parser, &iter->error);
		return 1;
	}
	i_unreached();
}