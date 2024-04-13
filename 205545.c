rsvg_filter_primitive_image_set_atts (RsvgNode *node, gpointer impl, RsvgHandle *handle, RsvgPropertyBag *atts)
{
    RsvgFilterPrimitiveImage *filter = impl;
    const char *value;

    filter->ctx = handle;

    if ((value = rsvg_property_bag_lookup (atts, "in")))
        g_string_assign (filter->super.in, value);
    if ((value = rsvg_property_bag_lookup (atts, "result")))
        g_string_assign (filter->super.result, value);
    if ((value = rsvg_property_bag_lookup (atts, "xlink:href"))) {
        filter->href = g_string_new (NULL);
        g_string_assign (filter->href, value);
    }

    filter_primitive_set_x_y_width_height_atts ((RsvgFilterPrimitive *) filter, atts);
}