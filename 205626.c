rsvg_new_filter_primitive_component_transfer (const char *element_name, RsvgNode *parent)
{
    RsvgFilterPrimitiveComponentTransfer *filter;

    filter = g_new0 (RsvgFilterPrimitiveComponentTransfer, 1);
    filter->super.result = g_string_new ("none");
    filter->super.in = g_string_new ("none");
    filter->super.render = rsvg_filter_primitive_component_transfer_render;

    return rsvg_rust_cnode_new (RSVG_NODE_TYPE_FILTER_PRIMITIVE_COMPONENT_TRANSFER,
                                parent,
                                rsvg_state_new (),
                                filter,
                                rsvg_filter_primitive_component_transfer_set_atts,
                                rsvg_filter_draw,
                                rsvg_filter_primitive_free);                                
}