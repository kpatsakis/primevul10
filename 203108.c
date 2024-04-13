theme_adium_remove_focus_marks (EmpathyThemeAdium *theme,
    WebKitDOMNodeList *nodes)
{
	guint i;

	/* Remove focus and firstFocus class */
	for (i = 0; i < webkit_dom_node_list_get_length (nodes); i++) {
		WebKitDOMNode *node = webkit_dom_node_list_item (nodes, i);
		WebKitDOMHTMLElement *element = WEBKIT_DOM_HTML_ELEMENT (node);
		gchar *class_name;
		gchar **classes, **iter;
		GString *new_class_name;
		gboolean first = TRUE;

		if (element == NULL) {
			continue;
		}

		class_name = webkit_dom_html_element_get_class_name (element);
		classes = g_strsplit (class_name, " ", -1);
		new_class_name = g_string_sized_new (strlen (class_name));
		for (iter = classes; *iter != NULL; iter++) {
			if (tp_strdiff (*iter, "focus") &&
			    tp_strdiff (*iter, "firstFocus")) {
				if (!first) {
					g_string_append_c (new_class_name, ' ');
				}
				g_string_append (new_class_name, *iter);
				first = FALSE;
			}
		}

		webkit_dom_html_element_set_class_name (element, new_class_name->str);

		g_free (class_name);
		g_strfreev (classes);
		g_string_free (new_class_name, TRUE);
	}
}