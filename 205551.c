rsvg_new_filter_primitive_erode (const char *element_name, RsvgNode *parent)
{
    RsvgFilterPrimitiveErode *filter;

    filter = g_new0 (RsvgFilterPrimitiveErode, 1);
    filter->super.in = g_string_new ("none");
    filter->super.result = g_string_new ("none");
    filter->rx = 0;
    filter->ry = 0;
    filter->mode = 0;
    filter->super.render = rsvg_filter_primitive_erode_render;

    return rsvg_rust_cnode_new (RSVG_NODE_TYPE_FILTER_PRIMITIVE_ERODE,
                                parent,
                                rsvg_state_new (),
                                filter,
                                rsvg_filter_primitive_erode_set_atts,
                                rsvg_filter_draw,
                                rsvg_filter_primitive_free);
}