rsvg_new_filter_primitive_merge (const char *element_name, RsvgNode *parent)
{
    RsvgFilterPrimitiveMerge *filter;

    filter = g_new0 (RsvgFilterPrimitiveMerge, 1);
    filter->super.result = g_string_new ("none");
    filter->super.render = rsvg_filter_primitive_merge_render;

    return rsvg_rust_cnode_new (RSVG_NODE_TYPE_FILTER_PRIMITIVE_MERGE,
                                parent,
                                rsvg_state_new (),
                                filter,
                                rsvg_filter_primitive_merge_set_atts,
                                rsvg_filter_draw,
                                rsvg_filter_primitive_free);
}