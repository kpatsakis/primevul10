rsvg_new_filter_primitive_displacement_map (const char *element_name, RsvgNode *parent)
{
    RsvgFilterPrimitiveDisplacementMap *filter;

    filter = g_new0 (RsvgFilterPrimitiveDisplacementMap, 1);
    filter->super.in = g_string_new ("none");
    filter->in2 = g_string_new ("none");
    filter->super.result = g_string_new ("none");
    filter->xChannelSelector = ' ';
    filter->yChannelSelector = ' ';
    filter->scale = 0;
    filter->super.render = rsvg_filter_primitive_displacement_map_render;

    return rsvg_rust_cnode_new (RSVG_NODE_TYPE_FILTER_PRIMITIVE_DISPLACEMENT_MAP,
                                parent,
                                rsvg_state_new (),
                                filter,
                                rsvg_filter_primitive_displacement_map_set_atts,
                                rsvg_filter_draw,
                                rsvg_filter_primitive_displacement_map_free);
}