rsvg_new_node_component_transfer_function (const char *element_name, RsvgNode *parent)
{
    RsvgNodeComponentTransferFunc *filter;

    char channel;

    if (strcmp (element_name, "feFuncR") == 0)
        channel = 'r';
    else if (strcmp (element_name, "feFuncG") == 0)
        channel = 'g';
    else if (strcmp (element_name, "feFuncB") == 0)
        channel = 'b';
    else if (strcmp (element_name, "feFuncA") == 0)
        channel = 'a';
    else {
        g_assert_not_reached ();
        channel = '\0';
    }

    filter = g_new0 (RsvgNodeComponentTransferFunc, 1);
    filter->function = identity_component_transfer_func;
    filter->nbTableValues = 0;
    filter->channel = channel;

    return rsvg_rust_cnode_new (RSVG_NODE_TYPE_COMPONENT_TRANFER_FUNCTION,
                                parent,
                                rsvg_state_new (),
                                filter,
                                rsvg_node_component_transfer_function_set_atts,
                                rsvg_filter_draw,
                                rsvg_node_component_transfer_function_free);
}