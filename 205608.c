rsvg_filter_primitive_flood_set_atts (RsvgNode *node, gpointer impl, RsvgHandle *handle, RsvgPropertyBag *atts)
{
    RsvgFilterPrimitive *filter = impl;
    const char *value;

    if ((value = rsvg_property_bag_lookup (atts, "result")))
        g_string_assign (filter->result, value);

    filter_primitive_set_x_y_width_height_atts ((RsvgFilterPrimitive *) filter, atts);
}