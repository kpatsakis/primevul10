rsvg_filter_primitive_offset_set_atts (RsvgNode *node, gpointer impl, RsvgHandle *handle, RsvgPropertyBag * atts)
{
    RsvgFilterPrimitiveOffset *filter = impl;
    const char *value;

    if ((value = rsvg_property_bag_lookup (atts, "in")))
        g_string_assign (filter->super.in, value);
    if ((value = rsvg_property_bag_lookup (atts, "result")))
        g_string_assign (filter->super.result, value);

    filter_primitive_set_x_y_width_height_atts ((RsvgFilterPrimitive *) filter, atts);

    if ((value = rsvg_property_bag_lookup (atts, "dx")))
        filter->dx = rsvg_length_parse (value, LENGTH_DIR_HORIZONTAL);
    if ((value = rsvg_property_bag_lookup (atts, "dy")))
        filter->dy = rsvg_length_parse (value, LENGTH_DIR_VERTICAL);
}