rsvg_filter_primitive_image_render (RsvgNode *node, RsvgFilterPrimitive *primitive, RsvgFilterContext *ctx)
{
    RsvgFilterPrimitiveImage *image = (RsvgFilterPrimitiveImage *) primitive;

    RsvgIRect boundarys;
    RsvgFilterPrimitiveOutput op;
    cairo_surface_t *output, *img;

    if (!image->href)
        return;

    boundarys = rsvg_filter_primitive_get_bounds (primitive, ctx);

    output = _rsvg_image_surface_new (ctx->width, ctx->height);
    if (output == NULL)
        return;

    img = rsvg_filter_primitive_image_render_in (image, ctx);
    if (img == NULL) {
        img = rsvg_filter_primitive_image_render_ext (primitive, ctx);
    }

    if (img) {
        cairo_t *cr;

        cr = cairo_create (output);
        cairo_set_source_surface (cr, img, 0, 0);
        cairo_rectangle (cr,
                         boundarys.x0,
                         boundarys.y0,
                         boundarys.x1 - boundarys.x0,
                         boundarys.y1 - boundarys.y0);
        cairo_clip (cr);
        cairo_paint (cr);
        cairo_destroy (cr);

        cairo_surface_destroy (img);
    }

    op.surface = output;
    op.bounds = boundarys;

    rsvg_filter_store_output (primitive->result, op, ctx);

    cairo_surface_destroy (output);
}