_rsvg_node_circle_draw (RsvgNode * self, RsvgDrawingCtx * ctx, int dominate)
{
    GString *d = NULL;
    cairo_path_t *path;
    RsvgNodeCircle *circle = (RsvgNodeCircle *) self;
    char buf[G_ASCII_DTOSTR_BUF_SIZE];
    double cx, cy, r;

    cx = _rsvg_css_normalize_length (&circle->cx, ctx, 'h');
    cy = _rsvg_css_normalize_length (&circle->cy, ctx, 'v');
    r = _rsvg_css_normalize_length (&circle->r, ctx, 'o');

    if (r <= 0)
        return;

    /* approximate a circle using 4 bezier curves */

    d = g_string_new ("M ");
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx + r));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy));

    g_string_append (d, " C ");
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx + r));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy + r * RSVG_ARC_MAGIC));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx + r * RSVG_ARC_MAGIC));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy + r));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy + r));

    g_string_append (d, " C ");
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx - r * RSVG_ARC_MAGIC));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy + r));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx - r));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy + r * RSVG_ARC_MAGIC));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx - r));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy));

    g_string_append (d, " C ");
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx - r));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy - r * RSVG_ARC_MAGIC));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx - r * RSVG_ARC_MAGIC));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy - r));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy - r));

    g_string_append (d, " C ");
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx + r * RSVG_ARC_MAGIC));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy - r));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx + r));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy - r * RSVG_ARC_MAGIC));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cx + r));
    g_string_append_c (d, ' ');
    g_string_append (d, g_ascii_dtostr (buf, sizeof (buf), cy));

    g_string_append (d, " Z");

    rsvg_state_reinherit_top (ctx, self->state, dominate);

    path = rsvg_parse_path (d->str);
    rsvg_render_path (ctx, path);
    rsvg_cairo_path_destroy (path);

    g_string_free (d, TRUE);
}