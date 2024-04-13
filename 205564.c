rsvg_filter_primitive_tile_render (RsvgNode *node, RsvgFilterPrimitive *primitive, RsvgFilterContext *ctx)
{
    guchar i;
    gint x, y, rowstride;
    RsvgIRect boundarys, oboundarys;

    RsvgFilterPrimitiveOutput input;

    guchar *in_pixels;
    guchar *output_pixels;

    cairo_surface_t *output, *in;

    oboundarys = rsvg_filter_primitive_get_bounds (primitive, ctx);

    input = rsvg_filter_get_result (primitive->in, ctx);
    in = input.surface;
    boundarys = input.bounds;

    cairo_surface_flush (in);

    in_pixels = cairo_image_surface_get_data (in);

    output = _rsvg_image_surface_new (ctx->width, ctx->height);
    if (output == NULL) {
        cairo_surface_destroy (in);
        return;
    }

    rowstride = cairo_image_surface_get_stride (output);

    output_pixels = cairo_image_surface_get_data (output);

    for (y = oboundarys.y0; y < oboundarys.y1; y++)
        for (x = oboundarys.x0; x < oboundarys.x1; x++)
            for (i = 0; i < 4; i++) {
                output_pixels[4 * x + y * rowstride + i] =
                    in_pixels[(mod ((x - boundarys.x0), (boundarys.x1 - boundarys.x0)) +
                               boundarys.x0) * 4 +
                              (mod ((y - boundarys.y0), (boundarys.y1 - boundarys.y0)) +
                               boundarys.y0) * rowstride + i];
            }

    cairo_surface_mark_dirty (output);

    rsvg_filter_store_result (primitive->result, output, ctx);

    cairo_surface_destroy (in);
    cairo_surface_destroy (output);
}