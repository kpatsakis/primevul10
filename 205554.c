rsvg_filter_primitive_gaussian_blur_set_atts (RsvgNode *node, gpointer impl, RsvgHandle *handle, RsvgPropertyBag *atts)
{
    RsvgFilterPrimitiveGaussianBlur *filter = impl;
    const char *value;

    if ((value = rsvg_property_bag_lookup (atts, "in")))
        g_string_assign (filter->super.in, value);
    if ((value = rsvg_property_bag_lookup (atts, "result")))
        g_string_assign (filter->super.result, value);

    filter_primitive_set_x_y_width_height_atts ((RsvgFilterPrimitive *) filter, atts);

    if ((value = rsvg_property_bag_lookup (atts, "stdDeviation"))) {
        if (!rsvg_css_parse_number_optional_number (value, &filter->sdx, &filter->sdy)) {
            rsvg_node_set_attribute_parse_error (node, "stdDeviation", "expected number-optional-number");
            return;
        }
    }
}