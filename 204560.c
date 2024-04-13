_rsvg_node_ellipse_draw (RsvgNode * self, RsvgDrawingCtx * ctx, int dominate)
{
    RsvgNodeEllipse *ellipse = (RsvgNodeEllipse *) self;
    GString *d = NULL;
    cairo_path_t *path;
    char buf[G_ASCII_DTOSTR_BUF_SIZE];
    double cx, cy, rx, ry;

    cx = _rsvg_css_normalize_length (&ellipse->cx, ctx, 'h');
    cy = _rsvg_css_normalize_length (&ellipse->cy, ctx, 'v');
    rx = _rsvg_css_normalize_length (&ellipse->rx, ctx, 'h');
    ry = _rsvg_css_normalize_length (&ellipse->ry, ctx, 'v');

    if (rx <= 0 || ry <= 0)
        return;
    /* approximate an ellipse using 4 bezier curves */

    d = g_string_new ("M ");
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx + rx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy));

    g_string_append (d, " C ");
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx + rx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy - RSVG_ARC_MAGIC * ry));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx + RSVG_ARC_MAGIC * rx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy - ry));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy - ry));

    g_string_append (d, " C ");
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx - RSVG_ARC_MAGIC * rx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy - ry));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx - rx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy - RSVG_ARC_MAGIC * ry));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx - rx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy));

    g_string_append (d, " C ");
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx - rx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy + RSVG_ARC_MAGIC * ry));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx - RSVG_ARC_MAGIC * rx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy + ry));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy + ry));

    g_string_append (d, " C ");
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx + RSVG_ARC_MAGIC * rx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy + ry));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx + rx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy + RSVG_ARC_MAGIC * ry));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx + rx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy));

    g_string_append (d, " Z");

    rsvg_state_reinherit_top (ctx, self->state, dominate);

    path = rsvg_parse_path (d->str);
    rsvg_render_path (ctx, path);
    rsvg_cairo_path_destroy (path);

    g_string_free (d, TRUE);
}