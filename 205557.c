rsvg_filter_set_atts (RsvgNode *node, gpointer impl, RsvgHandle *handle, RsvgPropertyBag *atts)
{
    RsvgFilter *filter = impl;
    const char *value;

    if ((value = rsvg_property_bag_lookup (atts, "filterUnits"))) {
        if (!strcmp (value, "userSpaceOnUse"))
            filter->filterunits = userSpaceOnUse;
        else
            filter->filterunits = objectBoundingBox;
    }
    if ((value = rsvg_property_bag_lookup (atts, "primitiveUnits"))) {
        if (!strcmp (value, "objectBoundingBox"))
            filter->primitiveunits = objectBoundingBox;
        else
            filter->primitiveunits = userSpaceOnUse;
    }
    if ((value = rsvg_property_bag_lookup (atts, "x")))
        filter->x = rsvg_length_parse (value, LENGTH_DIR_HORIZONTAL);
    if ((value = rsvg_property_bag_lookup (atts, "y")))
        filter->y = rsvg_length_parse (value, LENGTH_DIR_VERTICAL);
    if ((value = rsvg_property_bag_lookup (atts, "width")))
        filter->width = rsvg_length_parse (value, LENGTH_DIR_HORIZONTAL);
    if ((value = rsvg_property_bag_lookup (atts, "height")))
        filter->height = rsvg_length_parse (value, LENGTH_DIR_VERTICAL);
}