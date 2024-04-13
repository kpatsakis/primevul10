merge_render_child (RsvgNode *node, gpointer data)
{
    struct merge_render_closure *closure = data;
    RsvgFilterPrimitive *fp;
    cairo_surface_t *in;

    if (rsvg_node_get_type (node) != RSVG_NODE_TYPE_FILTER_PRIMITIVE_MERGE_NODE)
        return TRUE;

    fp = rsvg_rust_cnode_get_impl (node);

    in = rsvg_filter_get_in (fp->in, closure->ctx);
    if (in == NULL)
        return TRUE;

    rsvg_alpha_blt (in,
                    closure->boundarys.x0,
                    closure->boundarys.y0,
                    closure->boundarys.x1 - closure->boundarys.x0,
                    closure->boundarys.y1 - closure->boundarys.y0,
                    closure->output,
                    closure->boundarys.x0,
                    closure->boundarys.y0);

    cairo_surface_destroy (in);

    return TRUE;
}