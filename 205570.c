rsvg_new_filter_primitive_blend (const char *element_name, RsvgNode *parent)
{
    RsvgFilterPrimitiveBlend *filter;

    filter = g_new0 (RsvgFilterPrimitiveBlend, 1);
    filter->mode = normal;
    filter->super.in = g_string_new ("none");
    filter->in2 = g_string_new ("none");
    filter->super.result = g_string_new ("none");
    filter->super.render = rsvg_filter_primitive_blend_render;

    return rsvg_rust_cnode_new (RSVG_NODE_TYPE_FILTER_PRIMITIVE_BLEND,
                                parent,
                                rsvg_state_new (),
                                filter,
                                rsvg_filter_primitive_blend_set_atts,
                                rsvg_filter_draw,
                                rsvg_filter_primitive_blend_free);
}