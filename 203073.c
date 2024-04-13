theme_adium_navigation_policy_decision_requested_cb (WebKitWebView             *view,
						     WebKitWebFrame            *web_frame,
						     WebKitNetworkRequest      *request,
						     WebKitWebNavigationAction *action,
						     WebKitWebPolicyDecision   *decision,
						     gpointer                   data)
{
	const gchar *uri;

	/* Only call url_show on clicks */
	if (webkit_web_navigation_action_get_reason (action) !=
	    WEBKIT_WEB_NAVIGATION_REASON_LINK_CLICKED) {
		webkit_web_policy_decision_use (decision);
		return TRUE;
	}

	uri = webkit_network_request_get_uri (request);
	empathy_url_show (GTK_WIDGET (view), uri);

	webkit_web_policy_decision_ignore (decision);
	return TRUE;
}