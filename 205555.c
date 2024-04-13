rsvg_filter_primitive_displacement_map_set_atts (RsvgNode *node, gpointer impl, RsvgHandle *handle, RsvgPropertyBag *atts)
{
    RsvgFilterPrimitiveDisplacementMap *filter = impl;
    const char *value;

    if ((value = rsvg_property_bag_lookup (atts, "in")))
        g_string_assign (filter->super.in, value);
    if ((value = rsvg_property_bag_lookup (atts, "in2")))
        g_string_assign (filter->in2, value);
    if ((value = rsvg_property_bag_lookup (atts, "result")))
        g_string_assign (filter->super.result, value);

    filter_primitive_set_x_y_width_height_atts ((RsvgFilterPrimitive *) filter, atts);

    if ((value = rsvg_property_bag_lookup (atts, "xChannelSelector")))
        filter->xChannelSelector = (value)[0];
    if ((value = rsvg_property_bag_lookup (atts, "yChannelSelector")))
        filter->yChannelSelector = (value)[0];
    if ((value = rsvg_property_bag_lookup (atts, "scale")))
        filter->scale = g_ascii_strtod (value, NULL);
}