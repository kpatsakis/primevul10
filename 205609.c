rsvg_filter_primitive_convolve_matrix_render (RsvgNode *node, RsvgFilterPrimitive *primitive, RsvgFilterContext *ctx)
{
    RsvgFilterPrimitiveConvolveMatrix *convolve = (RsvgFilterPrimitiveConvolveMatrix *) primitive;

    guchar ch;
    gint x, y;
    gint i, j;
    gint rowstride, height, width;
    RsvgIRect boundarys;

    guchar *in_pixels;
    guchar *output_pixels;

    cairo_surface_t *output, *in;

    gint sx, sy, kx, ky;
    guchar sval;
    double kval, sum, dx, dy, targetx, targety;
    int umch;

    gint tempresult;

    boundarys = rsvg_filter_primitive_get_bounds (primitive, ctx);

    in = rsvg_filter_get_in (primitive->in, ctx);
    if (in == NULL)
        return;

    cairo_surface_flush (in);

    in_pixels = cairo_image_surface_get_data (in);

    height = cairo_image_surface_get_height (in);
    width = cairo_image_surface_get_width (in);

    targetx = convolve->targetx * ctx->paffine.xx;
    targety = convolve->targety * ctx->paffine.yy;

    if (convolve->dx != 0 || convolve->dy != 0) {
        dx = convolve->dx * ctx->paffine.xx;
        dy = convolve->dy * ctx->paffine.yy;
    } else
        dx = dy = 1;

    rowstride = cairo_image_surface_get_stride (in);

    output = _rsvg_image_surface_new (width, height);
    if (output == NULL) {
        cairo_surface_destroy (in);
        return;
    }

    output_pixels = cairo_image_surface_get_data (output);

    for (y = boundarys.y0; y < boundarys.y1; y++) {
        for (x = boundarys.x0; x < boundarys.x1; x++) {
            for (umch = 0; umch < 3 + !convolve->preservealpha; umch++) {
                ch = ctx->channelmap[umch];
                sum = 0;
                for (i = 0; i < convolve->ordery; i++) {
                    for (j = 0; j < convolve->orderx; j++) {
                        int alpha;
                        sx = x - targetx + j * dx;
                        sy = y - targety + i * dy;
                        if (convolve->edgemode == EDGE_MODE_DUPLICATE) {
                            if (sx < boundarys.x0)
                                sx = boundarys.x0;
                            if (sx >= boundarys.x1)
                                sx = boundarys.x1 - 1;
                            if (sy < boundarys.y0)
                                sy = boundarys.y0;
                            if (sy >= boundarys.y1)
                                sy = boundarys.y1 - 1;
                        } else if (convolve->edgemode == EDGE_MODE_WRAP) {
                            if (sx < boundarys.x0 || (sx >= boundarys.x1))
                                sx = boundarys.x0 + (sx - boundarys.x0) %
                                    (boundarys.x1 - boundarys.x0);
                            if (sy < boundarys.y0 || (sy >= boundarys.y1))
                                sy = boundarys.y0 + (sy - boundarys.y0) %
                                    (boundarys.y1 - boundarys.y0);
                        } else if (convolve->edgemode == EDGE_MODE_NONE) {
                            if (sx < boundarys.x0 || (sx >= boundarys.x1) ||
                                sy < boundarys.y0 || (sy >= boundarys.y1))
                                continue;
                        } else {
                            g_assert_not_reached ();
                        }

                        kx = convolve->orderx - j - 1;
                        ky = convolve->ordery - i - 1;
                        alpha = in_pixels[4 * sx + sy * rowstride + 3];
                        if (ch == 3)
                            sval = alpha;
                        else if (alpha)
                            sval = in_pixels[4 * sx + sy * rowstride + ch] * 255 / alpha;
                        else
                            sval = 0;
                        kval = convolve->KernelMatrix[kx + ky * convolve->orderx];
                        sum += (double) sval *kval;
                    }
                }

                tempresult = sum / convolve->divisor + convolve->bias;

                if (tempresult > 255)
                    tempresult = 255;
                if (tempresult < 0)
                    tempresult = 0;

                output_pixels[4 * x + y * rowstride + ch] = tempresult;
            }
            if (convolve->preservealpha)
                output_pixels[4 * x + y * rowstride + ctx->channelmap[3]] =
                    in_pixels[4 * x + y * rowstride + ctx->channelmap[3]];
            for (umch = 0; umch < 3; umch++) {
                ch = ctx->channelmap[umch];
                output_pixels[4 * x + y * rowstride + ch] =
                    output_pixels[4 * x + y * rowstride + ch] *
                    output_pixels[4 * x + y * rowstride + ctx->channelmap[3]] / 255;
            }
        }
    }

    cairo_surface_mark_dirty (output);

    rsvg_filter_store_result (primitive->result, output, ctx);

    cairo_surface_destroy (in);
    cairo_surface_destroy (output);
}