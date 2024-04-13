rsvg_node_light_source_set_atts (RsvgNode *node, gpointer impl, RsvgHandle *handle, RsvgPropertyBag *atts)
{
    RsvgNodeLightSource *data = impl;
    const char *value;

    if ((value = rsvg_property_bag_lookup (atts, "azimuth")))
        data->azimuth = g_ascii_strtod (value, NULL) / 180.0 * M_PI;
    if ((value = rsvg_property_bag_lookup (atts, "elevation")))
        data->elevation = g_ascii_strtod (value, NULL) / 180.0 * M_PI;
    if ((value = rsvg_property_bag_lookup (atts, "limitingConeAngle")))
        data->limitingconeAngle = g_ascii_strtod (value, NULL) / 180.0 * M_PI;
    if ((value = rsvg_property_bag_lookup (atts, "x")))
        data->x = data->pointsAtX = rsvg_length_parse (value, LENGTH_DIR_HORIZONTAL);
    if ((value = rsvg_property_bag_lookup (atts, "y")))
        data->y = data->pointsAtX = rsvg_length_parse (value, LENGTH_DIR_VERTICAL);
    if ((value = rsvg_property_bag_lookup (atts, "z")))
        data->z = data->pointsAtX = rsvg_length_parse (value, LENGTH_DIR_BOTH);
    if ((value = rsvg_property_bag_lookup (atts, "pointsAtX")))
        data->pointsAtX = rsvg_length_parse (value, LENGTH_DIR_HORIZONTAL);
    if ((value = rsvg_property_bag_lookup (atts, "pointsAtY")))
        data->pointsAtY = rsvg_length_parse (value, LENGTH_DIR_VERTICAL);
    if ((value = rsvg_property_bag_lookup (atts, "pointsAtZ")))
        data->pointsAtZ = rsvg_length_parse (value, LENGTH_DIR_BOTH);
    if ((value = rsvg_property_bag_lookup (atts, "specularExponent")))
        data->specularExponent = g_ascii_strtod (value, NULL);
}