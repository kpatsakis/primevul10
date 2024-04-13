rsvg_new_filter_primitive_composite (const char *element_name, RsvgNode *parent)
{
    RsvgFilterPrimitiveComposite *filter;

    filter = g_new0 (RsvgFilterPrimitiveComposite, 1);
    filter->mode = COMPOSITE_MODE_OVER;
    filter->super.in = g_string_new ("none");
    filter->in2 = g_string_new ("none");
    filter->super.result = g_string_new ("none");
    filter->k1 = 0;
    filter->k2 = 0;
    filter->k3 = 0;
    filter->k4 = 0;
    filter->super.render = rsvg_filter_primitive_composite_render;

    return rsvg_rust_cnode_new (RSVG_NODE_TYPE_FILTER_PRIMITIVE_COMPOSITE,
                                parent,
                                rsvg_state_new (),
                                filter,
                                rsvg_filter_primitive_composite_set_atts,
                                rsvg_filter_draw,
                                rsvg_filter_primitive_composite_free);
}