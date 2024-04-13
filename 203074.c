theme_adium_copy_clipboard (EmpathyChatView *view)
{
	webkit_web_view_copy_clipboard (WEBKIT_WEB_VIEW (view));
}