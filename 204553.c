_rsvg_node_rect_draw (RsvgNode * self, RsvgDrawingCtx * ctx, int dominate)
{
    double x, y, w, h, rx, ry;
    GString *d = NULL;
    cairo_path_t *path;
    RsvgNodeRect *rect = (RsvgNodeRect *) self;
    char buf[G_ASCII_DTOSTR_BUF_SIZE];

    x = _rsvg_css_normalize_length (&rect->x, ctx, 'h');
    y = _rsvg_css_normalize_length (&rect->y, ctx, 'v');
    w = _rsvg_css_normalize_length (&rect->w, ctx, 'h');
    h = _rsvg_css_normalize_length (&rect->h, ctx, 'v');
    rx = _rsvg_css_normalize_length (&rect->rx, ctx, 'h');
    ry = _rsvg_css_normalize_length (&rect->ry, ctx, 'v');

    if (w == 0. || h == 0.)
	return;

    if (rect->got_rx)
        rx = rx;
    else
        rx = ry;
    if (rect->got_ry)
        ry = ry;
    else
        ry = rx;

    if (rx > fabs (w / 2.))
        rx = fabs (w / 2.);
    if (ry > fabs (h / 2.))
        ry = fabs (h / 2.);

    if (rx == 0)
        ry = 0;
    else if (ry == 0)
        rx = 0;

    /* emulate a rect using a path */
    d = g_string_new ("M ");
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), x + rx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), y));

    g_string_append (d, " H ");
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), x + w - rx));

    g_string_append (d, " A");
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), rx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), ry));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), 0.));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), 0.));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), 1.));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), x + w));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), y + ry));

    g_string_append (d, " V ");
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), y + h - ry));

    g_string_append (d, " A");
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), rx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), ry));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), 0.));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), 0.));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), 1.));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), x + w - rx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), y + h));

    g_string_append (d, " H ");
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), x + rx));

    g_string_append (d, " A");
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), rx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), ry));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), 0.));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), 0.));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), 1.));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), x));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), y + h - ry));

    g_string_append (d, " V ");
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), y + ry));

    g_string_append (d, " A");
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), rx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), ry));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), 0.));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), 0.));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), 1.));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), x + rx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), y));

    g_string_append (d, " Z");

    rsvg_state_reinherit_top (ctx, self->state, dominate);

    path = rsvg_parse_path (d->str);
    rsvg_render_path (ctx, path);
    rsvg_cairo_path_destroy (path);
    g_string_free (d, TRUE);
}