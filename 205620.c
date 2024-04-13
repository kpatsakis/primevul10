rsvg_filter_primitive_erode_render (RsvgNode *node, RsvgFilterPrimitive *primitive, RsvgFilterContext *ctx)
{
    RsvgFilterPrimitiveErode *erode = (RsvgFilterPrimitiveErode *) primitive;

    guchar ch, extreme;
    gint x, y;
    gint i, j;
    gint rowstride, height, width;
    RsvgIRect boundarys;

    guchar *in_pixels;
    guchar *output_pixels;

    cairo_surface_t *output, *in;

    gint kx, ky;
    guchar val;

    boundarys = rsvg_filter_primitive_get_bounds (primitive, ctx);

    in = rsvg_filter_get_in (primitive->in, ctx);
    if (in == NULL)
        return;

    cairo_surface_flush (in);

    in_pixels = cairo_image_surface_get_data (in);

    height = cairo_image_surface_get_height (in);
    width = cairo_image_surface_get_width (in);

    rowstride = cairo_image_surface_get_stride (in);

    /* scale the radius values */
    kx = erode->rx * ctx->paffine.xx;
    ky = erode->ry * ctx->paffine.yy;

    output = _rsvg_image_surface_new (width, height);
    if (output == NULL) {
        cairo_surface_destroy (in);
        return;
    }

    output_pixels = cairo_image_surface_get_data (output);

    for (y = boundarys.y0; y < boundarys.y1; y++)
        for (x = boundarys.x0; x < boundarys.x1; x++)
            for (ch = 0; ch < 4; ch++) {
                if (erode->mode == 0)
                    extreme = 255;
                else
                    extreme = 0;
                for (i = -ky; i < ky + 1; i++)
                    for (j = -kx; j < kx + 1; j++) {
                        if (y + i >= height || y + i < 0 || x + j >= width || x + j < 0)
                            continue;

                        val = in_pixels[(y + i) * rowstride + (x + j) * 4 + ch];


                        if (erode->mode == 0) {
                            if (extreme > val)
                                extreme = val;
                        } else {
                            if (extreme < val)
                                extreme = val;
                        }

                    }
                output_pixels[y * rowstride + x * 4 + ch] = extreme;
            }

    cairo_surface_mark_dirty (output);

    rsvg_filter_store_result (primitive->result, output, ctx);

    cairo_surface_destroy (in);
    cairo_surface_destroy (output);
}