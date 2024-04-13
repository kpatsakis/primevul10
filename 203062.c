theme_adium_remove_all_focus_marks (EmpathyThemeAdium *theme)
{
	EmpathyThemeAdiumPriv *priv = GET_PRIV (theme);
	WebKitDOMDocument *dom;
	WebKitDOMNodeList *nodes;
	GError *error = NULL;

	if (!priv->has_unread_message)
		return;

	priv->has_unread_message = FALSE;

	dom = webkit_web_view_get_dom_document (WEBKIT_WEB_VIEW (theme));
	if (dom == NULL) {
		return;
	}

	/* Get all nodes with focus class */
	nodes = webkit_dom_document_query_selector_all (dom, ".focus", &error);
	if (nodes == NULL) {
		DEBUG ("Error getting focus nodes: %s",
			error ? error->message : "No error");
		g_clear_error (&error);
		return;
	}

	theme_adium_remove_focus_marks (theme, nodes);
}