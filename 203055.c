theme_adium_append_event_markup (EmpathyChatView *view,
				 const gchar     *markup_text,
				 const gchar     *fallback_text)
{
	theme_adium_append_event_escaped (view, markup_text);
}