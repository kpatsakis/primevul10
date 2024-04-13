rsvg_filter_render (RsvgNode *filter_node,
                    cairo_surface_t *source,
                    RsvgDrawingCtx *context,
                    RsvgBbox *bounds,
                    char *channelmap)
{
    RsvgFilter *filter;
    RsvgFilterContext *ctx;
    guint i;
    cairo_surface_t *output;

    g_return_val_if_fail (source != NULL, NULL);
    g_return_val_if_fail (cairo_surface_get_type (source) == CAIRO_SURFACE_TYPE_IMAGE, NULL);

    g_assert (rsvg_node_get_type (filter_node) == RSVG_NODE_TYPE_FILTER);
    filter = rsvg_rust_cnode_get_impl (filter_node);

    ctx = g_new0 (RsvgFilterContext, 1);
    ctx->filter = filter;
    ctx->source_surface = source;
    ctx->bg_surface = NULL;
    ctx->results = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, rsvg_filter_free_pair);
    ctx->ctx = context;

    rsvg_filter_fix_coordinate_system (ctx, rsvg_current_state (context), bounds);

    ctx->lastresult.surface = cairo_surface_reference (source);
    ctx->lastresult.bounds = rsvg_filter_primitive_get_bounds (NULL, ctx);

    for (i = 0; i < 4; i++)
        ctx->channelmap[i] = channelmap[i] - '0';

    rsvg_node_foreach_child (filter_node, render_child_if_filter_primitive, ctx);

    output = ctx->lastresult.surface;

    g_hash_table_destroy (ctx->results);

    rsvg_filter_context_free (ctx);

    return output;
}