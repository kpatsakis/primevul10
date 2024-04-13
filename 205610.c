rsvg_filter_primitive_composite_render (RsvgNode *node, RsvgFilterPrimitive *primitive, RsvgFilterContext *ctx)
{
    RsvgFilterPrimitiveComposite *composite = (RsvgFilterPrimitiveComposite *) primitive;
    RsvgIRect boundarys;
    cairo_surface_t *output, *in, *in2;

    boundarys = rsvg_filter_primitive_get_bounds (primitive, ctx);

    in = rsvg_filter_get_in (primitive->in, ctx);
    if (in == NULL)
        return;

    in2 = rsvg_filter_get_in (composite->in2, ctx);
    if (in2 == NULL) {
        cairo_surface_destroy (in);
        return;
    }

    if (composite->mode == COMPOSITE_MODE_ARITHMETIC) {
        guchar i;
        gint x, y;
        gint rowstride, height, width;
        guchar *in_pixels;
        guchar *in2_pixels;
        guchar *output_pixels;

        height = cairo_image_surface_get_height (in);
        width = cairo_image_surface_get_width (in);
        rowstride = cairo_image_surface_get_stride (in);

        output = _rsvg_image_surface_new (width, height);
        if (output == NULL) {
            cairo_surface_destroy (in);
            cairo_surface_destroy (in2);
            return;
        }

        cairo_surface_flush (in);
        cairo_surface_flush (in2);

        in_pixels = cairo_image_surface_get_data (in);
        in2_pixels = cairo_image_surface_get_data (in2);
        output_pixels = cairo_image_surface_get_data (output);

        for (y = boundarys.y0; y < boundarys.y1; y++) {
            for (x = boundarys.x0; x < boundarys.x1; x++) {
                int qr, qa, qb;

                qa = in_pixels[4 * x + y * rowstride + 3];
                qb = in2_pixels[4 * x + y * rowstride + 3];
                qr = (composite->k1 * qa * qb / 255 + composite->k2 * qa + composite->k3 * qb) / 255;

                if (qr > 255)
                    qr = 255;
                if (qr < 0)
                    qr = 0;
                output_pixels[4 * x + y * rowstride + 3] = qr;
                if (qr) {
                    for (i = 0; i < 3; i++) {
                        int ca, cb, cr;
                        ca = in_pixels[4 * x + y * rowstride + i];
                        cb = in2_pixels[4 * x + y * rowstride + i];

                        cr = (ca * cb * composite->k1 / 255 + ca * composite->k2 +
                              cb * composite->k3 + composite->k4 * qr) / 255;
                        if (cr > qr)
                            cr = qr;
                        if (cr < 0)
                            cr = 0;
                        output_pixels[4 * x + y * rowstride + i] = cr;
                    }
                }
            }
        }

        cairo_surface_mark_dirty (output);
    } else {
        cairo_t *cr;

        cairo_surface_reference (in2);
        output = in2;

        cr = cairo_create (output);
        cairo_set_source_surface (cr, in, 0, 0);
        cairo_rectangle (cr,
                         boundarys.x0,
                         boundarys.y0,
                         boundarys.x1 - boundarys.x0,
                         boundarys.y1 - boundarys.y0);
        cairo_clip (cr);
        cairo_set_operator (cr, composite_mode_to_cairo_operator (composite->mode));
        cairo_paint (cr);
        cairo_destroy (cr);
    }

    rsvg_filter_store_result (primitive->result, output, ctx);

    cairo_surface_destroy (in);
    cairo_surface_destroy (in2);
    cairo_surface_destroy (output);
}