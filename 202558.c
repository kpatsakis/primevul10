output_title_label_expose_event_cb (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
        GsdXrandrManager *manager = GSD_XRANDR_MANAGER (data);
        struct GsdXrandrManagerPrivate *priv = manager->priv;
        GnomeOutputInfo *output;
        GdkColor color;
        cairo_t *cr;

        g_assert (GTK_IS_LABEL (widget));

        output = g_object_get_data (G_OBJECT (widget), "output");
        g_assert (output != NULL);

        g_assert (priv->labeler != NULL);

        /* Draw a black rectangular border, filled with the color that corresponds to this output */

        gnome_rr_labeler_get_color_for_output (priv->labeler, output, &color);

        cr = gdk_cairo_create (widget->window);

        cairo_set_source_rgb (cr, 0, 0, 0);
        cairo_set_line_width (cr, OUTPUT_TITLE_ITEM_BORDER);
        cairo_rectangle (cr,
                         widget->allocation.x + OUTPUT_TITLE_ITEM_BORDER / 2.0,
                         widget->allocation.y + OUTPUT_TITLE_ITEM_BORDER / 2.0,
                         widget->allocation.width - OUTPUT_TITLE_ITEM_BORDER,
                         widget->allocation.height - OUTPUT_TITLE_ITEM_BORDER);
        cairo_stroke (cr);

        gdk_cairo_set_source_color (cr, &color);
        cairo_rectangle (cr,
                         widget->allocation.x + OUTPUT_TITLE_ITEM_BORDER,
                         widget->allocation.y + OUTPUT_TITLE_ITEM_BORDER,
                         widget->allocation.width - 2 * OUTPUT_TITLE_ITEM_BORDER,
                         widget->allocation.height - 2 * OUTPUT_TITLE_ITEM_BORDER);

        cairo_fill (cr);

        /* We want the label to always show up as if it were sensitive
         * ("style->fg[GTK_STATE_NORMAL]"), even though the label is insensitive
         * due to being inside an insensitive menu item.  So, here we have a
         * HACK in which we frob the label's state directly.  GtkLabel's expose
         * handler will be run after this function, so it will think that the
         * label is in GTK_STATE_NORMAL.  We reset the label's state back to
         * insensitive in output_title_label_after_expose_event_cb().
         *
         * Yay for fucking with GTK+'s internals.
         */

        widget->state = GTK_STATE_NORMAL;

        return FALSE;
}