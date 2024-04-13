empathy_adium_data_new_with_info (const gchar *path, GHashTable *info)
{
	EmpathyAdiumData *data;
	gchar            *template_html = NULL;
	gchar            *footer_html = NULL;
	gchar            *tmp;

	g_return_val_if_fail (empathy_adium_path_is_valid (path), NULL);

	data = g_slice_new0 (EmpathyAdiumData);
	data->ref_count = 1;
	data->path = g_strdup (path);
	data->basedir = g_strconcat (path, G_DIR_SEPARATOR_S "Contents"
		G_DIR_SEPARATOR_S "Resources" G_DIR_SEPARATOR_S, NULL);
	data->info = g_hash_table_ref (info);
	data->version = adium_info_get_version (info);
	data->strings_to_free = g_ptr_array_new_with_free_func (g_free);
	data->date_format_cache = g_hash_table_new_full (g_str_hash,
		g_str_equal, g_free, g_free);

	DEBUG ("Loading theme at %s", path);

#define LOAD(path, var) \
		tmp = g_build_filename (data->basedir, path, NULL); \
		g_file_get_contents (tmp, &var, NULL, NULL); \
		g_free (tmp); \

#define LOAD_CONST(path, var) \
	{ \
		gchar *content; \
		LOAD (path, content); \
		if (content != NULL) { \
			g_ptr_array_add (data->strings_to_free, content); \
		} \
		var = content; \
	}

	/* Load html files */
	LOAD_CONST ("Content.html", data->content_html);
	LOAD_CONST ("Incoming/Content.html", data->in_content_html);
	LOAD_CONST ("Incoming/NextContent.html", data->in_nextcontent_html);
	LOAD_CONST ("Incoming/Context.html", data->in_context_html);
	LOAD_CONST ("Incoming/NextContext.html", data->in_nextcontext_html);
	LOAD_CONST ("Outgoing/Content.html", data->out_content_html);
	LOAD_CONST ("Outgoing/NextContent.html", data->out_nextcontent_html);
	LOAD_CONST ("Outgoing/Context.html", data->out_context_html);
	LOAD_CONST ("Outgoing/NextContext.html", data->out_nextcontext_html);
	LOAD_CONST ("Status.html", data->status_html);
	LOAD ("Template.html", template_html);
	LOAD ("Footer.html", footer_html);

#undef LOAD_CONST
#undef LOAD

	/* HTML fallbacks: If we have at least content OR in_content, then
	 * everything else gets a fallback */

#define FALLBACK(html, fallback) \
	if (html == NULL) { \
		html = fallback; \
	}

	/* in_nextcontent -> in_content -> content */
	FALLBACK (data->in_content_html,      data->content_html);
	FALLBACK (data->in_nextcontent_html,  data->in_content_html);

	/* context -> content */
	FALLBACK (data->in_context_html,      data->in_content_html);
	FALLBACK (data->in_nextcontext_html,  data->in_nextcontent_html);
	FALLBACK (data->out_context_html,     data->out_content_html);
	FALLBACK (data->out_nextcontext_html, data->out_nextcontent_html);

	/* out -> in */
	FALLBACK (data->out_content_html,     data->in_content_html);
	FALLBACK (data->out_nextcontent_html, data->in_nextcontent_html);
	FALLBACK (data->out_context_html,     data->in_context_html);
	FALLBACK (data->out_nextcontext_html, data->in_nextcontext_html);

	/* status -> in_content */
	FALLBACK (data->status_html,          data->in_content_html);

#undef FALLBACK

	/* template -> empathy's template */
	data->custom_template = (template_html != NULL);
	if (template_html == NULL) {
		GError *error = NULL;

		tmp = empathy_file_lookup ("Template.html", "data");

		if (!g_file_get_contents (tmp, &template_html, NULL, &error)) {
			g_warning ("couldn't load Empathy's default theme "
				"template: %s", error->message);
			g_return_val_if_reached (data);
		}

		g_free (tmp);
	}

	/* Default avatar */
	tmp = g_build_filename (data->basedir, "Incoming", "buddy_icon.png", NULL);
	if (g_file_test (tmp, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR)) {
		data->default_incoming_avatar_filename = tmp;
	} else {
		g_free (tmp);
	}
	tmp = g_build_filename (data->basedir, "Outgoing", "buddy_icon.png", NULL);
	if (g_file_test (tmp, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR)) {
		data->default_outgoing_avatar_filename = tmp;
	} else {
		g_free (tmp);
	}

	/* Old custom templates had only 4 parameters.
	 * New templates have 5 parameters */
	if (data->version <= 2 && data->custom_template) {
		tmp = string_with_format (template_html,
			data->basedir,
			"%@", /* Leave variant unset */
			"", /* The header */
			footer_html ? footer_html : "",
			NULL);
	} else {
		tmp = string_with_format (template_html,
			data->basedir,
			data->version <= 2 ? "" : "@import url( \"main.css\" );",
			"%@", /* Leave variant unset */
			"", /* The header */
			footer_html ? footer_html : "",
			NULL);
	}
	g_ptr_array_add (data->strings_to_free, tmp);
	data->template_html = tmp;

	g_free (template_html);
	g_free (footer_html);

	return data;