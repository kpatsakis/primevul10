rsvg_filter_primitive_offset_render (RsvgNode *node, RsvgFilterPrimitive *primitive, RsvgFilterContext *ctx)
{
    RsvgFilterPrimitiveOffset *offset = (RsvgFilterPrimitiveOffset *) primitive;

    guchar ch;
    gint x, y;
    gint rowstride, height, width;
    RsvgIRect boundarys;

    guchar *in_pixels;
    guchar *output_pixels;

    RsvgFilterPrimitiveOutput out;

    cairo_surface_t *output, *in;

    double dx, dy;
    int ox, oy;

    boundarys = rsvg_filter_primitive_get_bounds (primitive, ctx);

    in = rsvg_filter_get_in (primitive->in, ctx);
    if (in == NULL)
        return;

    cairo_surface_flush (in);

    in_pixels = cairo_image_surface_get_data (in);

    height = cairo_image_surface_get_height (in);
    width = cairo_image_surface_get_width (in);

    rowstride = cairo_image_surface_get_stride (in);

    output = _rsvg_image_surface_new (width, height);
    if (output == NULL) {
        cairo_surface_destroy (in);
        return;
    }

    output_pixels = cairo_image_surface_get_data (output);

    dx = rsvg_length_normalize (&offset->dx, ctx->ctx);
    dy = rsvg_length_normalize (&offset->dy, ctx->ctx);

    ox = ctx->paffine.xx * dx + ctx->paffine.xy * dy;
    oy = ctx->paffine.yx * dx + ctx->paffine.yy * dy;

    for (y = boundarys.y0; y < boundarys.y1; y++)
        for (x = boundarys.x0; x < boundarys.x1; x++) {
            if (x - ox < boundarys.x0 || x - ox >= boundarys.x1)
                continue;
            if (y - oy < boundarys.y0 || y - oy >= boundarys.y1)
                continue;

            for (ch = 0; ch < 4; ch++) {
                output_pixels[y * rowstride + x * 4 + ch] =
                    in_pixels[(y - oy) * rowstride + (x - ox) * 4 + ch];
            }
        }

    cairo_surface_mark_dirty (output);

    out.surface = output;
    out.bounds = boundarys;

    rsvg_filter_store_output (primitive->result, out, ctx);

    cairo_surface_destroy  (in);
    cairo_surface_destroy (output);
}