bool db_dict_value_iter_next(struct db_dict_value_iter *iter,
			     const char **key_r, const char **value_r)
{
	static struct var_expand_func_table var_funcs_table[] = {
		{ "dict", db_dict_field_find },
		{ NULL, NULL }
	};
	const struct db_dict_field *field;
	const char *error;

	if (iter->field_idx == array_count(iter->fields))
		return db_dict_value_iter_object_next(iter, key_r, value_r);
	field = array_idx(iter->fields, iter->field_idx++);

	str_truncate(iter->tmpstr, 0);
	if (var_expand_with_funcs(iter->tmpstr, field->value,
				  iter->var_expand_table, var_funcs_table,
				  iter, &error) <= 0) {
		iter->error = p_strdup_printf(iter->pool,
			"Failed to expand %s=%s: %s",
			field->name, field->value, error);
		return FALSE;
	}
	*key_r = field->name;
	*value_r = str_c(iter->tmpstr);
	return TRUE;
}