surface_get_alpha (cairo_surface_t *source,
                   RsvgFilterContext * ctx)
{
    guchar *data;
    guchar *pbdata;
    gsize i, pbsize;
    cairo_surface_t *surface;

    if (source == NULL)
        return NULL;

    cairo_surface_flush (source);

    pbsize = cairo_image_surface_get_width (source) *
             cairo_image_surface_get_height (source);

    surface = _rsvg_image_surface_new (cairo_image_surface_get_width (source),
                                       cairo_image_surface_get_height (source));
    if (surface == NULL)
        return NULL;

    data = cairo_image_surface_get_data (surface);
    pbdata = cairo_image_surface_get_data (source);

    /* FIXMEchpe: rewrite this into nested width, height loops */
    for (i = 0; i < pbsize; i++)
        data[i * 4 + ctx->channelmap[3]] = pbdata[i * 4 + ctx->channelmap[3]];

    cairo_surface_mark_dirty (surface);
    return surface;
}