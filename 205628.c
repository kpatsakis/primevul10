rsvg_new_filter_primitive_convolve_matrix (const char *element_name, RsvgNode *parent)
{
    RsvgFilterPrimitiveConvolveMatrix *filter;

    filter = g_new0 (RsvgFilterPrimitiveConvolveMatrix, 1);
    filter->super.in = g_string_new ("none");
    filter->super.result = g_string_new ("none");
    filter->KernelMatrix = NULL;
    filter->divisor = 0;
    filter->bias = 0;
    filter->dx = 0;
    filter->dy = 0;
    filter->preservealpha = FALSE;
    filter->edgemode = EDGE_MODE_DUPLICATE;
    filter->super.render = rsvg_filter_primitive_convolve_matrix_render;

    return rsvg_rust_cnode_new (RSVG_NODE_TYPE_FILTER_PRIMITIVE_CONVOLVE_MATRIX,
                                parent,
                                rsvg_state_new (),
                                filter,
                                rsvg_filter_primitive_convolve_matrix_set_atts,
                                rsvg_filter_draw,
                                rsvg_filter_primitive_convolve_matrix_free);
}