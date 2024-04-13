rsvg_filter_primitive_blend_set_atts (RsvgNode *node, gpointer impl, RsvgHandle *handle, RsvgPropertyBag *atts)
{
    RsvgFilterPrimitiveBlend *filter = impl;
    const char *value;

    if ((value = rsvg_property_bag_lookup (atts, "mode"))) {
        if (!strcmp (value, "multiply"))
            filter->mode = multiply;
        else if (!strcmp (value, "screen"))
            filter->mode = screen;
        else if (!strcmp (value, "darken"))
            filter->mode = darken;
        else if (!strcmp (value, "lighten"))
            filter->mode = lighten;
        else
            filter->mode = normal;
    }
    if ((value = rsvg_property_bag_lookup (atts, "in")))
        g_string_assign (filter->super.in, value);
    if ((value = rsvg_property_bag_lookup (atts, "in2")))
        g_string_assign (filter->in2, value);
    if ((value = rsvg_property_bag_lookup (atts, "result")))
        g_string_assign (filter->super.result, value);

    filter_primitive_set_x_y_width_height_atts ((RsvgFilterPrimitive *) filter, atts);
}