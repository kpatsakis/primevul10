rsvg_new_filter_primitive_color_matrix (const char *element_name, RsvgNode *parent)
{
    RsvgFilterPrimitiveColorMatrix *filter;

    filter = g_new0 (RsvgFilterPrimitiveColorMatrix, 1);
    filter->super.in = g_string_new ("none");
    filter->super.result = g_string_new ("none");
    filter->KernelMatrix = NULL;
    filter->super.render = rsvg_filter_primitive_color_matrix_render;

    return rsvg_rust_cnode_new (RSVG_NODE_TYPE_FILTER_PRIMITIVE_COLOR_MATRIX,
                                parent,
                                rsvg_state_new (),
                                filter,
                                rsvg_filter_primitive_color_matrix_set_atts,
                                rsvg_filter_draw,
                                rsvg_filter_primitive_color_matrix_free);                                
}