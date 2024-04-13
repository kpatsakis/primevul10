rsvg_new_filter_primitive_image (const char *element_name, RsvgNode *parent)
{
    RsvgFilterPrimitiveImage *filter;

    filter = g_new0 (RsvgFilterPrimitiveImage, 1);
    filter->super.in = g_string_new ("none");
    filter->super.result = g_string_new ("none");
    filter->super.render = rsvg_filter_primitive_image_render;
    filter->href = NULL;

    return rsvg_rust_cnode_new (RSVG_NODE_TYPE_FILTER_PRIMITIVE_IMAGE,
                                parent,
                                rsvg_state_new (),
                                filter,
                                rsvg_filter_primitive_image_set_atts,
                                rsvg_filter_draw,
                                rsvg_filter_primitive_image_free);
}