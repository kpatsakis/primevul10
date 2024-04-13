theme_adium_scroll_down (EmpathyChatView *view)
{
	webkit_web_view_execute_script (WEBKIT_WEB_VIEW (view), "alignChat(true);");
}