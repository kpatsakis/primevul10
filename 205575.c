rsvg_new_filter_primitive_flood (const char *element_name, RsvgNode *parent)
{
    RsvgFilterPrimitive *filter;

    filter = g_new0 (RsvgFilterPrimitive, 1);
    filter->in = g_string_new ("none");
    filter->result = g_string_new ("none");
    filter->render = rsvg_filter_primitive_flood_render;

    return rsvg_rust_cnode_new (RSVG_NODE_TYPE_FILTER_PRIMITIVE_FLOOD,
                                parent,
                                rsvg_state_new (),
                                filter,
                                rsvg_filter_primitive_flood_set_atts,
                                rsvg_filter_draw,
                                rsvg_filter_primitive_free);
}