theme_adium_get_has_selection (EmpathyChatView *view)
{
	return webkit_web_view_has_selection (WEBKIT_WEB_VIEW (view));
}