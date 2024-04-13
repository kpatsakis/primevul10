rsvg_filter_primitive_erode_set_atts (RsvgNode *node, gpointer impl, RsvgHandle *handle, RsvgPropertyBag *atts)
{
    RsvgFilterPrimitiveErode *filter = impl;
    const char *value;

    if ((value = rsvg_property_bag_lookup (atts, "in")))
        g_string_assign (filter->super.in, value);
    if ((value = rsvg_property_bag_lookup (atts, "result")))
        g_string_assign (filter->super.result, value);

    filter_primitive_set_x_y_width_height_atts ((RsvgFilterPrimitive *) filter, atts);

    if ((value = rsvg_property_bag_lookup (atts, "radius"))) {
        if (!rsvg_css_parse_number_optional_number (value, &filter->rx, &filter->ry)) {
            rsvg_node_set_attribute_parse_error (node, "radius", "expected number-optional-number");
            return;
        }
    }

    if ((value = rsvg_property_bag_lookup (atts, "operator"))) {
        if (!strcmp (value, "erode"))
            filter->mode = 0;
        else if (!strcmp (value, "dilate"))
            filter->mode = 1;
    }
}