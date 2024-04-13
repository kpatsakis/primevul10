event_filter (GdkXEvent           *xevent,
              GdkEvent            *event,
              gpointer             data)
{
        GsdXrandrManager *manager = data;
        XEvent *xev = (XEvent *) xevent;

        if (!manager->priv->running)
                return GDK_FILTER_CONTINUE;

        /* verify we have a key event */
        if (xev->xany.type != KeyPress && xev->xany.type != KeyRelease)
                return GDK_FILTER_CONTINUE;

        if (xev->xany.type == KeyPress && xev->xkey.keycode == manager->priv->keycode) {
                handle_fn_f7 (manager, xev->xkey.time);

                return GDK_FILTER_CONTINUE;
        }

        return GDK_FILTER_CONTINUE;
}