rsvg_new_filter (const char *element_name, RsvgNode *parent)
{
    RsvgFilter *filter;

    filter = g_new0 (RsvgFilter, 1);
    filter->filterunits = objectBoundingBox;
    filter->primitiveunits = userSpaceOnUse;
    filter->x = rsvg_length_parse ("-10%", LENGTH_DIR_HORIZONTAL);
    filter->y = rsvg_length_parse ("-10%", LENGTH_DIR_VERTICAL);
    filter->width = rsvg_length_parse ("120%", LENGTH_DIR_HORIZONTAL);
    filter->height = rsvg_length_parse ("120%", LENGTH_DIR_VERTICAL);

    return rsvg_rust_cnode_new (RSVG_NODE_TYPE_FILTER,
                                parent,
                                rsvg_state_new (),
                                filter,
                                rsvg_filter_set_atts,
                                rsvg_filter_draw,
                                rsvg_filter_free);
}