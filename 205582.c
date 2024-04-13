rsvg_new_filter_primitive_offset (const char *element_name, RsvgNode *parent)
{
    RsvgFilterPrimitiveOffset *filter;

    filter = g_new0 (RsvgFilterPrimitiveOffset, 1);
    filter->super.in = g_string_new ("none");
    filter->super.result = g_string_new ("none");
    filter->dx = rsvg_length_parse ("0", LENGTH_DIR_HORIZONTAL);
    filter->dy = rsvg_length_parse ("0", LENGTH_DIR_VERTICAL);
    filter->super.render = rsvg_filter_primitive_offset_render;

    return rsvg_rust_cnode_new (RSVG_NODE_TYPE_FILTER_PRIMITIVE_OFFSET,
                                parent,
                                rsvg_state_new (),
                                filter,
                                rsvg_filter_primitive_offset_set_atts,
                                rsvg_filter_draw,
                                rsvg_filter_primitive_free);
}