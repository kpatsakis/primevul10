rsvg_filter_primitive_image_render_ext (RsvgFilterPrimitive *self, RsvgFilterContext * ctx)
{
    RsvgFilterPrimitiveImage *image = (RsvgFilterPrimitiveImage *) self;
    RsvgIRect boundarys;
    cairo_surface_t *img, *intermediate;
    int i;
    unsigned char *pixels;
    int channelmap[4];
    int length;
    int width, height;

    if (!image->href)
        return NULL;

    boundarys = rsvg_filter_primitive_get_bounds (self, ctx);

    width = boundarys.x1 - boundarys.x0;
    height = boundarys.y1 - boundarys.y0;
    if (width == 0 || height == 0)
        return NULL;

    img = rsvg_cairo_surface_new_from_href (image->ctx,
                                            image->href->str,
                                            NULL);
    if (!img)
        return NULL;

    intermediate = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
    if (cairo_surface_status (intermediate) != CAIRO_STATUS_SUCCESS ||
        !rsvg_art_affine_image (img, intermediate,
                                &ctx->paffine,
                                (gdouble) width / ctx->paffine.xx,
                                (gdouble) height / ctx->paffine.yy)) {
        cairo_surface_destroy (intermediate);
        cairo_surface_destroy (img);
        return NULL;
    }

    cairo_surface_destroy (img);

    length = cairo_image_surface_get_height (intermediate) *
             cairo_image_surface_get_stride (intermediate);
    for (i = 0; i < 4; i++)
        channelmap[i] = ctx->channelmap[i];
    pixels = cairo_image_surface_get_data (intermediate);
    for (i = 0; i < length; i += 4) {
        unsigned char alpha;
        unsigned char pixel[4];
        int ch;
        alpha = pixels[i + 3];

        pixel[channelmap[3]] = alpha;
        if (alpha)
            for (ch = 0; ch < 3; ch++)
                pixel[channelmap[ch]] = pixels[i + ch] * alpha / 255;
        else
            for (ch = 0; ch < 3; ch++)
                pixel[channelmap[ch]] = 0;
        for (ch = 0; ch < 4; ch++)
            pixels[i + ch] = pixel[ch];
    }

    cairo_surface_mark_dirty (intermediate);
    return intermediate;
}