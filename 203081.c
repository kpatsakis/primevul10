theme_adium_edit_message (EmpathyChatView *view,
			  EmpathyMessage  *message)
{
	EmpathyThemeAdiumPriv *priv = GET_PRIV (view);
	WebKitDOMDocument *doc;
	WebKitDOMElement *span;
	gchar *id, *parsed_body;
	gchar *tooltip, *timestamp;
	GtkIconInfo *icon_info;
	GError *error = NULL;

	if (priv->pages_loading != 0) {
		queue_item (&priv->message_queue, QUEUED_EDIT, message, NULL);
		return;
	}

	id = g_strdup_printf ("message-token-%s",
		empathy_message_get_supersedes (message));
	/* we don't pass a token here, because doing so will return another
	 * <span> element, and we don't want nested <span> elements */
	parsed_body = theme_adium_parse_body (EMPATHY_THEME_ADIUM (view),
		empathy_message_get_body (message), NULL);

	/* find the element */
	doc = webkit_web_view_get_dom_document (WEBKIT_WEB_VIEW (view));
	span = webkit_dom_document_get_element_by_id (doc, id);

	if (span == NULL) {
		DEBUG ("Failed to find id '%s'", id);
		goto except;
	}

	if (!WEBKIT_DOM_IS_HTML_ELEMENT (span)) {
		DEBUG ("Not a HTML element");
		goto except;
	}

	/* update the HTML */
	webkit_dom_html_element_set_inner_html (WEBKIT_DOM_HTML_ELEMENT (span),
		parsed_body, &error);

	if (error != NULL) {
		DEBUG ("Error setting new inner-HTML: %s", error->message);
		g_error_free (error);
		goto except;
	}

	/* set a tooltip */
	timestamp = empathy_time_to_string_local (
		empathy_message_get_timestamp (message),
		"%H:%M:%S");
	tooltip = g_strdup_printf (_("Message edited at %s"), timestamp);

	webkit_dom_html_element_set_title (WEBKIT_DOM_HTML_ELEMENT (span),
		tooltip);

	g_free (tooltip);
	g_free (timestamp);

	/* mark this message as edited */
	icon_info = gtk_icon_theme_lookup_icon (gtk_icon_theme_get_default (),
		EMPATHY_IMAGE_EDIT_MESSAGE, 16, 0);

	if (icon_info != NULL) {
		/* set the icon as a background image using CSS
		 * FIXME: the icon won't update in response to theme changes */
		gchar *style = g_strdup_printf (
			"background-image:url('%s');"
			"background-repeat:no-repeat;"
			"background-position:left center;"
			"padding-left:19px;", /* 16px icon + 3px padding */
			gtk_icon_info_get_filename (icon_info));

		webkit_dom_element_set_attribute (span, "style", style, &error);

		if (error != NULL) {
			DEBUG ("Error setting element style: %s",
				error->message);
			g_clear_error (&error);
			/* not fatal */
		}

		g_free (style);
		gtk_icon_info_free (icon_info);
	}

	goto finally;

except:
	DEBUG ("Could not find message to edit with: %s",
		empathy_message_get_body (message));

finally:
	g_free (id);
	g_free (parsed_body);
}