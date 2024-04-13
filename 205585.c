rsvg_filter_blend (RsvgFilterPrimitiveBlendMode mode,
                   cairo_surface_t *in,
                   cairo_surface_t *in2,
                   cairo_surface_t* output,
                   RsvgIRect boundarys,
                   int *channelmap)
{
    guchar i;
    gint x, y;
    gint rowstride, rowstride2, rowstrideo, height, width;
    guchar *in_pixels;
    guchar *in2_pixels;
    guchar *output_pixels;

    cairo_surface_flush (in);
    cairo_surface_flush (in2);

    height = cairo_image_surface_get_height (in);
    width = cairo_image_surface_get_width (in);
    rowstride = cairo_image_surface_get_stride (in);
    rowstride2 = cairo_image_surface_get_stride (in2);
    rowstrideo = cairo_image_surface_get_stride (output);

    output_pixels = cairo_image_surface_get_data (output);
    in_pixels = cairo_image_surface_get_data (in);
    in2_pixels = cairo_image_surface_get_data (in2);

    if (boundarys.x0 < 0)
        boundarys.x0 = 0;
    if (boundarys.y0 < 0)
        boundarys.y0 = 0;
    if (boundarys.x1 >= width)
        boundarys.x1 = width;
    if (boundarys.y1 >= height)
        boundarys.y1 = height;

    for (y = boundarys.y0; y < boundarys.y1; y++)
        for (x = boundarys.x0; x < boundarys.x1; x++) {
            double qr, cr, qa, qb, ca, cb, bca, bcb;
            int ch;

            qa = (double) in_pixels[4 * x + y * rowstride + channelmap[3]] / 255.0;
            qb = (double) in2_pixels[4 * x + y * rowstride2 + channelmap[3]] / 255.0;
            qr = 1 - (1 - qa) * (1 - qb);
            cr = 0;
            for (ch = 0; ch < 3; ch++) {
                i = channelmap[ch];
                ca = (double) in_pixels[4 * x + y * rowstride + i] / 255.0;
                cb = (double) in2_pixels[4 * x + y * rowstride2 + i] / 255.0;
                /*these are the ca and cb that are used in the non-standard blend functions */
                bcb = (1 - qa) * cb + ca;
                bca = (1 - qb) * ca + cb;
                switch (mode) {
                case normal:
                    cr = (1 - qa) * cb + ca;
                    break;
                case multiply:
                    cr = (1 - qa) * cb + (1 - qb) * ca + ca * cb;
                    break;
                case screen:
                    cr = cb + ca - ca * cb;
                    break;
                case darken:
                    cr = MIN ((1 - qa) * cb + ca, (1 - qb) * ca + cb);
                    break;
                case lighten:
                    cr = MAX ((1 - qa) * cb + ca, (1 - qb) * ca + cb);
                    break;
                case softlight:
                    if (bcb < 0.5)
                        cr = 2 * bca * bcb + bca * bca * (1 - 2 * bcb);
                    else
                        cr = sqrt (bca) * (2 * bcb - 1) + (2 * bca) * (1 - bcb);
                    break;
                case hardlight:
                    if (cb < 0.5)
                        cr = 2 * bca * bcb;
                    else
                        cr = 1 - 2 * (1 - bca) * (1 - bcb);
                    break;
                case colordodge:
                    if (bcb == 1)
                        cr = 1;
                    else
                        cr = MIN (bca / (1 - bcb), 1);
                    break;
                case colorburn:
                    if (bcb == 0)
                        cr = 0;
                    else
                        cr = MAX (1 - (1 - bca) / bcb, 0);
                    break;
                case overlay:
                    if (bca < 0.5)
                        cr = 2 * bca * bcb;
                    else
                        cr = 1 - 2 * (1 - bca) * (1 - bcb);
                    break;
                case exclusion:
                    cr = bca + bcb - 2 * bca * bcb;
                    break;
                case difference:
                    cr = abs (bca - bcb);
                    break;
                }
                cr *= 255.0;
                if (cr > 255)
                    cr = 255;
                if (cr < 0)
                    cr = 0;
                output_pixels[4 * x + y * rowstrideo + i] = (guchar) cr;

            }
            output_pixels[4 * x + y * rowstrideo + channelmap[3]] = qr * 255.0;
        }

    cairo_surface_mark_dirty (output);
}