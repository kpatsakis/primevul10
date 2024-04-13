error_message (GsdXrandrManager *mgr, const char *primary_text, GError *error_to_display, const char *secondary_text)
{
#ifdef HAVE_LIBNOTIFY
        GsdXrandrManagerPrivate *priv = mgr->priv;
        NotifyNotification *notification;

        g_assert (error_to_display == NULL || secondary_text == NULL);

        if (priv->status_icon)
                notification = notify_notification_new_with_status_icon (primary_text,
                                                                         error_to_display ? error_to_display->message : secondary_text,
                                                                         GSD_XRANDR_ICON_NAME,
                                                                         priv->status_icon);
        else
                notification = notify_notification_new (primary_text,
                                                        error_to_display ? error_to_display->message : secondary_text,
                                                        GSD_XRANDR_ICON_NAME,
                                                        NULL);

        notify_notification_show (notification, NULL); /* NULL-GError */
#else
        GtkWidget *dialog;

	dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                         "%s", primary_text);
        gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%s",
                                                  error_to_display ? error_to_display->message : secondary_text);

        gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);
#endif /* HAVE_LIBNOTIFY */
}