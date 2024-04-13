render_child_if_filter_primitive (RsvgNode *node, gpointer data)
{
    RsvgFilterContext *filter_ctx = data;

    if (node_is_filter_primitive (node)) {
        RsvgFilterPrimitive *primitive;

        primitive = rsvg_rust_cnode_get_impl (node);
        rsvg_filter_primitive_render (node, primitive, filter_ctx);
    }

    return TRUE;
}