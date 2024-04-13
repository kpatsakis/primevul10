component_transfer_render_child (RsvgNode *node, gpointer data)
{
    struct component_transfer_closure *closure = data;
    RsvgNodeComponentTransferFunc *f;

    if (rsvg_node_get_type (node) != RSVG_NODE_TYPE_COMPONENT_TRANFER_FUNCTION)
        return TRUE;

    f = rsvg_rust_cnode_get_impl (node);

    if (f->channel == closure->channel) {
        closure->functions[closure->ctx->channelmap[closure->channel_num]] = f->function;
        closure->channels[closure->ctx->channelmap[closure->channel_num]] = f;
        closure->set_func = TRUE;
    }

    return TRUE;
}