rsvg_new_filter_primitive_specular_lighting (const char *element_name, RsvgNode *parent)
{
    RsvgFilterPrimitiveSpecularLighting *filter;

    filter = g_new0 (RsvgFilterPrimitiveSpecularLighting, 1);
    filter->super.in = g_string_new ("none");
    filter->super.result = g_string_new ("none");
    filter->surfaceScale = 1;
    filter->specularConstant = 1;
    filter->specularExponent = 1;
    filter->lightingcolor = 0xFFFFFFFF;
    filter->super.render = rsvg_filter_primitive_specular_lighting_render;

    return rsvg_rust_cnode_new (RSVG_NODE_TYPE_FILTER_PRIMITIVE_SPECULAR_LIGHTING,
                                parent,
                                rsvg_state_new (),
                                filter,
                                rsvg_filter_primitive_specular_lighting_set_atts,
                                rsvg_filter_draw,
                                rsvg_filter_primitive_free);
}