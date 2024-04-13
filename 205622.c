rsvg_filter_primitive_merge_render (RsvgNode *node, RsvgFilterPrimitive *primitive, RsvgFilterContext *ctx)
{
    struct merge_render_closure closure;

    closure.boundarys = rsvg_filter_primitive_get_bounds (primitive, ctx);

    closure.output = _rsvg_image_surface_new (ctx->width, ctx->height);
    if (closure.output == NULL) {
        return;
    }

    closure.ctx = ctx;

    rsvg_node_foreach_child (node, merge_render_child, &closure);

    rsvg_filter_store_result (primitive->result, closure.output, ctx);

    cairo_surface_destroy (closure.output);
}