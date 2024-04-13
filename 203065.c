theme_adium_highlight (EmpathyChatView *view,
		       const gchar     *text,
		       gboolean         match_case)
{
	webkit_web_view_unmark_text_matches (WEBKIT_WEB_VIEW (view));
	webkit_web_view_mark_text_matches (WEBKIT_WEB_VIEW (view),
					   text, match_case, 0);
	webkit_web_view_set_highlight_text_matches (WEBKIT_WEB_VIEW (view),
						    TRUE);
}