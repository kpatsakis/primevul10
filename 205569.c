rsvg_new_filter_primitive_turbulence (const char *element_name, RsvgNode *parent)
{
    RsvgFilterPrimitiveTurbulence *filter;

    filter = g_new0 (RsvgFilterPrimitiveTurbulence, 1);
    filter->super.in = g_string_new ("none");
    filter->super.result = g_string_new ("none");
    filter->fBaseFreqX = 0;
    filter->fBaseFreqY = 0;
    filter->nNumOctaves = 1;
    filter->seed = 0;
    filter->bDoStitching = 0;
    filter->bFractalSum = 0;

    feTurbulence_init (filter);

    filter->super.render = rsvg_filter_primitive_turbulence_render;

    return rsvg_rust_cnode_new (RSVG_NODE_TYPE_FILTER_PRIMITIVE_TURBULENCE,
                                parent,
                                rsvg_state_new (),
                                filter,
                                rsvg_filter_primitive_turbulence_set_atts,
                                rsvg_filter_draw,
                                rsvg_filter_primitive_free);
}