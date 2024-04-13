rsvg_new_filter_primitive_tile (const char *element_name, RsvgNode *parent)
{
    RsvgFilterPrimitiveTile *filter;

    filter = g_new0 (RsvgFilterPrimitiveTile, 1);
    filter->super.in = g_string_new ("none");
    filter->super.result = g_string_new ("none");
    filter->super.render = rsvg_filter_primitive_tile_render;

    return rsvg_rust_cnode_new (RSVG_NODE_TYPE_FILTER_PRIMITIVE_TILE,
                                parent,
                                rsvg_state_new (),
                                filter,
                                rsvg_filter_primitive_tile_set_atts,
                                rsvg_filter_draw,
                                rsvg_filter_primitive_free);
}