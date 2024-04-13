rsvg_filter_primitive_flood_render (RsvgNode *node, RsvgFilterPrimitive *primitive, RsvgFilterContext *ctx)
{
    RsvgState *state;
    guchar i;
    gint x, y;
    gint rowstride, height, width;
    RsvgIRect boundarys;
    guchar *output_pixels;
    cairo_surface_t *output;
    char pixcolor[4];
    RsvgFilterPrimitiveOutput out;

    state = rsvg_node_get_state (node);

    guint32 color = state->flood_color;
    guint8 opacity = state->flood_opacity;

    boundarys = rsvg_filter_primitive_get_bounds (primitive, ctx);

    height = ctx->height;
    width = ctx->width;
    output = _rsvg_image_surface_new (width, height);
    if (output == NULL)
        return;

    rowstride = cairo_image_surface_get_stride (output);

    output_pixels = cairo_image_surface_get_data (output);

    for (i = 0; i < 3; i++)
        pixcolor[i] = (int) (((unsigned char *)
                              (&color))[2 - i]) * opacity / 255;
    pixcolor[3] = opacity;

    for (y = boundarys.y0; y < boundarys.y1; y++)
        for (x = boundarys.x0; x < boundarys.x1; x++)
            for (i = 0; i < 4; i++)
                output_pixels[4 * x + y * rowstride + ctx->channelmap[i]] = pixcolor[i];

    cairo_surface_mark_dirty (output);

    out.surface = output;
    out.bounds = boundarys;

    rsvg_filter_store_output (primitive->result, out, ctx);

    cairo_surface_destroy (output);
}