theme_adium_remove_acked_message_unread_mark_foreach (gpointer data,
						      gpointer user_data)
{
	EmpathyThemeAdium *self = user_data;
	guint32 id = GPOINTER_TO_UINT (data);

	theme_adium_remove_mark_from_message (self, id);
}