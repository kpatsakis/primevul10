rsvg_filter_store_result (GString * name,
                          cairo_surface_t *surface,
                          RsvgFilterContext * ctx)
{
    RsvgFilterPrimitiveOutput output;
    output.bounds.x0 = 0;
    output.bounds.y0 = 0;
    output.bounds.x1 = ctx->width;
    output.bounds.y1 = ctx->height;
    output.surface = surface;

    rsvg_filter_store_output (name, output, ctx);
}