part_write_bodystructure_common(const struct message_part_data *data,
				     string_t *str)
{
	str_append_c(str, ' ');
	if (data->content_disposition == NULL)
		str_append(str, "NIL");
	else {
		str_append_c(str, '(');
		imap_append_string(str, data->content_disposition);

		str_append_c(str, ' ');
		params_write(data->content_disposition_params,
			data->content_disposition_params_count, str, FALSE);

		str_append_c(str, ')');
	}

	str_append_c(str, ' ');
	if (data->content_language == NULL)
		str_append(str, "NIL");
	else {
		const char *const *lang = data->content_language;

		i_assert(*lang != NULL);
		str_append_c(str, '(');
		imap_append_string(str, *lang);
		lang++;
		while (*lang != NULL) {
			str_append_c(str, ' ');
			imap_append_string(str, *lang);
			lang++;
		}
		str_append_c(str, ')');
	}

	str_append_c(str, ' ');
	imap_append_nstring_nolf(str, data->content_location);
}