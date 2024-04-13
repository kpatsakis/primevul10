rsvg_filter_primitive_blend_render (RsvgNode *node, RsvgFilterPrimitive *primitive, RsvgFilterContext *ctx)
{
    RsvgFilterPrimitiveBlend *blend = (RsvgFilterPrimitiveBlend *) primitive;
    RsvgIRect boundarys;
    cairo_surface_t *output, *in, *in2;

    boundarys = rsvg_filter_primitive_get_bounds (primitive, ctx);

    in = rsvg_filter_get_in (primitive->in, ctx);
    if (in == NULL)
      return;

    in2 = rsvg_filter_get_in (blend->in2, ctx);
    if (in2 == NULL) {
        cairo_surface_destroy (in);
        return;
    }

    output = _rsvg_image_surface_new (cairo_image_surface_get_width (in),
                                      cairo_image_surface_get_height (in));
    if (output == NULL) {
        cairo_surface_destroy (in);
        cairo_surface_destroy (in2);
        return;
    }

    rsvg_filter_blend (blend->mode, in, in2, output, boundarys, ctx->channelmap);

    rsvg_filter_store_result (primitive->result, output, ctx);

    cairo_surface_destroy (in);
    cairo_surface_destroy (in2);
    cairo_surface_destroy (output);
}