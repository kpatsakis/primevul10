rsvg_new_filter_primitive_gaussian_blur (const char *element_name, RsvgNode *parent)
{
    RsvgFilterPrimitiveGaussianBlur *filter;

    filter = g_new0 (RsvgFilterPrimitiveGaussianBlur, 1);
    filter->super.in = g_string_new ("none");
    filter->super.result = g_string_new ("none");
    filter->sdx = 0;
    filter->sdy = 0;
    filter->super.render = rsvg_filter_primitive_gaussian_blur_render;

    return rsvg_rust_cnode_new (RSVG_NODE_TYPE_FILTER_PRIMITIVE_GAUSSIAN_BLUR,
                                parent,
                                rsvg_state_new (),
                                filter,
                                rsvg_filter_primitive_gaussian_blur_set_atts,
                                rsvg_filter_draw,
                                rsvg_filter_primitive_free);
}