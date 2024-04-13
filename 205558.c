rsvg_filter_primitive_turbulence_set_atts (RsvgNode *node, gpointer impl, RsvgHandle *handle, RsvgPropertyBag *atts)
{
    RsvgFilterPrimitiveTurbulence *filter = impl;
    const char *value;

    if ((value = rsvg_property_bag_lookup (atts, "in")))
        g_string_assign (filter->super.in, value);
    if ((value = rsvg_property_bag_lookup (atts, "result")))
        g_string_assign (filter->super.result, value);

    filter_primitive_set_x_y_width_height_atts ((RsvgFilterPrimitive *) filter, atts);

    if ((value = rsvg_property_bag_lookup (atts, "baseFrequency"))) {
        if (!rsvg_css_parse_number_optional_number (value, &filter->fBaseFreqX, &filter->fBaseFreqY)) {
            rsvg_node_set_attribute_parse_error (node, "baseFrequency", "expected number-optional-number");
            return;
        }
    }

    if ((value = rsvg_property_bag_lookup (atts, "numOctaves")))
        filter->nNumOctaves = atoi (value);
    if ((value = rsvg_property_bag_lookup (atts, "seed")))
        filter->seed = atoi (value);
    if ((value = rsvg_property_bag_lookup (atts, "stitchTiles")))
        filter->bDoStitching = (!strcmp (value, "stitch"));
    if ((value = rsvg_property_bag_lookup (atts, "type")))
        filter->bFractalSum = (!strcmp (value, "fractalNoise"));
}