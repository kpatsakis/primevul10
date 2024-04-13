theme_adium_remove_mark_from_message (EmpathyThemeAdium *self,
				      guint32 id)
{
	WebKitDOMDocument *dom;
	WebKitDOMNodeList *nodes;
	gchar *class;
	GError *error = NULL;

	dom = webkit_web_view_get_dom_document (WEBKIT_WEB_VIEW (self));
	if (dom == NULL) {
		return;
	}

	class = g_strdup_printf (".x-empathy-message-id-%u", id);

	/* Get all nodes with focus class */
	nodes = webkit_dom_document_query_selector_all (dom, class, &error);
	g_free (class);

	if (nodes == NULL) {
		DEBUG ("Error getting focus nodes: %s",
			error ? error->message : "No error");
		g_clear_error (&error);
		return;
	}

	theme_adium_remove_focus_marks (self, nodes);
}