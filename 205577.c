rsvg_filter_primitive_turbulence_render (RsvgNode *node, RsvgFilterPrimitive *primitive, RsvgFilterContext *ctx)
{
    RsvgFilterPrimitiveTurbulence *turbulence = (RsvgFilterPrimitiveTurbulence *) primitive;

    gint x, y, tileWidth, tileHeight, rowstride, width, height;
    RsvgIRect boundarys;
    guchar *output_pixels;
    cairo_surface_t *output, *in;
    cairo_matrix_t affine;

    affine = ctx->paffine;
    if (cairo_matrix_invert (&affine) != CAIRO_STATUS_SUCCESS)
      return;

    in = rsvg_filter_get_in (primitive->in, ctx);
    if (in == NULL)
        return;

    cairo_surface_flush (in);

    height = cairo_image_surface_get_height (in);
    width = cairo_image_surface_get_width (in);
    rowstride = cairo_image_surface_get_stride (in);

    boundarys = rsvg_filter_primitive_get_bounds (primitive, ctx);

    tileWidth = (boundarys.x1 - boundarys.x0);
    tileHeight = (boundarys.y1 - boundarys.y0);

    output = _rsvg_image_surface_new (width, height);
    if (output == NULL) {
        cairo_surface_destroy (in);
        return;
    }

    output_pixels = cairo_image_surface_get_data (output);

    for (y = 0; y < tileHeight; y++) {
        for (x = 0; x < tileWidth; x++) {
            gint i;
            double point[2];
            guchar *pixel;
            point[0] = affine.xx * (x + boundarys.x0) + affine.xy * (y + boundarys.y0) + affine.x0;
            point[1] = affine.yx * (x + boundarys.x0) + affine.yy * (y + boundarys.y0) + affine.y0;

            pixel = output_pixels + 4 * (x + boundarys.x0) + (y + boundarys.y0) * rowstride;

            for (i = 0; i < 4; i++) {
                double cr;

                cr = feTurbulence_turbulence (turbulence, i, point, (double) x, (double) y,
                                              (double) tileWidth, (double) tileHeight);

                if (turbulence->bFractalSum)
                    cr = ((cr * 255.) + 255.) / 2.;
                else
                    cr = (cr * 255.);

                cr = CLAMP (cr, 0., 255.);

                pixel[ctx->channelmap[i]] = (guchar) cr;
            }
            for (i = 0; i < 3; i++)
                pixel[ctx->channelmap[i]] =
                    pixel[ctx->channelmap[i]] * pixel[ctx->channelmap[3]] / 255;

        }
    }

    cairo_surface_mark_dirty (output);

    rsvg_filter_store_result (primitive->result, output, ctx);

    cairo_surface_destroy (in);
    cairo_surface_destroy (output);
}