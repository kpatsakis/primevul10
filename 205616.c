rsvg_filter_primitive_displacement_map_render (RsvgNode *node, RsvgFilterPrimitive *primitive, RsvgFilterContext *ctx)
{
    RsvgFilterPrimitiveDisplacementMap *displacement_map = (RsvgFilterPrimitiveDisplacementMap *) primitive;
    guchar ch, xch, ych;
    gint x, y;
    gint rowstride, height, width;
    RsvgIRect boundarys;

    guchar *in_pixels;
    guchar *in2_pixels;
    guchar *output_pixels;

    cairo_surface_t *output, *in, *in2;

    double ox, oy;

    boundarys = rsvg_filter_primitive_get_bounds (primitive, ctx);

    in = rsvg_filter_get_in (primitive->in, ctx);
    if (in == NULL)
        return;

    cairo_surface_flush (in);

    in2 = rsvg_filter_get_in (displacement_map->in2, ctx);
    if (in2 == NULL) {
        cairo_surface_destroy (in);
        return;
    }

    cairo_surface_flush (in2);

    in_pixels = cairo_image_surface_get_data (in);
    in2_pixels = cairo_image_surface_get_data (in2);

    height = cairo_image_surface_get_height (in);
    width = cairo_image_surface_get_width (in);

    rowstride = cairo_image_surface_get_stride (in);

    output = _rsvg_image_surface_new (width, height);
    if (output == NULL) {
        cairo_surface_destroy (in);
        cairo_surface_destroy (in2);
        return;
    }

    output_pixels = cairo_image_surface_get_data (output);

    switch (displacement_map->xChannelSelector) {
    case 'R':
        xch = 0;
        break;
    case 'G':
        xch = 1;
        break;
    case 'B':
        xch = 2;
        break;
    case 'A':
        xch = 3;
        break;
    default:
        xch = 0;
        break;
    }

    switch (displacement_map->yChannelSelector) {
    case 'R':
        ych = 0;
        break;
    case 'G':
        ych = 1;
        break;
    case 'B':
        ych = 2;
        break;
    case 'A':
        ych = 3;
        break;
    default:
        ych = 1;
        break;
    }

    xch = ctx->channelmap[xch];
    ych = ctx->channelmap[ych];
    for (y = boundarys.y0; y < boundarys.y1; y++)
        for (x = boundarys.x0; x < boundarys.x1; x++) {
            if (xch != 4)
                ox = x + displacement_map->scale * ctx->paffine.xx *
                    ((double) in2_pixels[y * rowstride + x * 4 + xch] / 255.0 - 0.5);
            else
                ox = x;

            if (ych != 4)
                oy = y + displacement_map->scale * ctx->paffine.yy *
                    ((double) in2_pixels[y * rowstride + x * 4 + ych] / 255.0 - 0.5);
            else
                oy = y;

            for (ch = 0; ch < 4; ch++) {
                output_pixels[y * rowstride + x * 4 + ch] =
                    get_interp_pixel (in_pixels, ox, oy, ch, boundarys, rowstride);
            }
        }

    cairo_surface_mark_dirty (output);

    rsvg_filter_store_result (primitive->result, output, ctx);

    cairo_surface_destroy (in);
    cairo_surface_destroy (in2);
    cairo_surface_destroy (output);
}