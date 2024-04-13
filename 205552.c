rsvg_filter_primitive_composite_set_atts (RsvgNode *node, gpointer impl, RsvgHandle *handle, RsvgPropertyBag *atts)
{
    RsvgFilterPrimitiveComposite *filter = impl;
    const char *value;

    if ((value = rsvg_property_bag_lookup (atts, "operator"))) {
        if (!strcmp (value, "in"))
            filter->mode = COMPOSITE_MODE_IN;
        else if (!strcmp (value, "out"))
            filter->mode = COMPOSITE_MODE_OUT;
        else if (!strcmp (value, "atop"))
            filter->mode = COMPOSITE_MODE_ATOP;
        else if (!strcmp (value, "xor"))
            filter->mode = COMPOSITE_MODE_XOR;
        else if (!strcmp (value, "arithmetic"))
            filter->mode = COMPOSITE_MODE_ARITHMETIC;
        else
            filter->mode = COMPOSITE_MODE_OVER;
    }
    if ((value = rsvg_property_bag_lookup (atts, "in")))
        g_string_assign (filter->super.in, value);
    if ((value = rsvg_property_bag_lookup (atts, "in2")))
        g_string_assign (filter->in2, value);
    if ((value = rsvg_property_bag_lookup (atts, "result")))
        g_string_assign (filter->super.result, value);

    filter_primitive_set_x_y_width_height_atts ((RsvgFilterPrimitive *) filter, atts);

    if ((value = rsvg_property_bag_lookup (atts, "k1")))
        filter->k1 = g_ascii_strtod (value, NULL) * 255.;
    if ((value = rsvg_property_bag_lookup (atts, "k2")))
        filter->k2 = g_ascii_strtod (value, NULL) * 255.;
    if ((value = rsvg_property_bag_lookup (atts, "k3")))
        filter->k3 = g_ascii_strtod (value, NULL) * 255.;
    if ((value = rsvg_property_bag_lookup (atts, "k4")))
        filter->k4 = g_ascii_strtod (value, NULL) * 255.;
}