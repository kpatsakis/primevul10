output_title_label_after_expose_event_cb (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
        g_assert (GTK_IS_LABEL (widget));
        widget->state = GTK_STATE_INSENSITIVE;

        return FALSE;
}