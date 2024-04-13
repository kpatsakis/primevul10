theme_adium_scroll (EmpathyChatView *view,
		    gboolean         allow_scrolling)
{
	EmpathyThemeAdiumPriv *priv = GET_PRIV (view);

	priv->allow_scrolling = allow_scrolling;
	if (allow_scrolling) {
		empathy_chat_view_scroll_down (view);
	}
}