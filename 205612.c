rsvg_node_component_transfer_function_set_atts (RsvgNode *node, gpointer impl, RsvgHandle *handle, RsvgPropertyBag *atts)
{
    RsvgNodeComponentTransferFunc *data = impl;
    const char *value;

    if ((value = rsvg_property_bag_lookup (atts, "type"))) {
        if (!strcmp (value, "identity"))
            data->function = identity_component_transfer_func;
        else if (!strcmp (value, "table"))
            data->function = table_component_transfer_func;
        else if (!strcmp (value, "discrete"))
            data->function = discrete_component_transfer_func;
        else if (!strcmp (value, "linear"))
            data->function = linear_component_transfer_func;
        else if (!strcmp (value, "gamma"))
            data->function = gamma_component_transfer_func;
    }

    if ((value = rsvg_property_bag_lookup (atts, "tableValues"))) {
        unsigned int i;
        double *temp;
        if (!rsvg_css_parse_number_list (value,
                                         NUMBER_LIST_LENGTH_MAXIMUM,
                                         256,
                                         &temp,
                                         &data->nbTableValues)) {
            rsvg_node_set_attribute_parse_error (node, "tableValues", "invalid number list");
            return;
        }

        data->tableValues = g_new0 (gint, data->nbTableValues);
        for (i = 0; i < data->nbTableValues; i++)
            data->tableValues[i] = temp[i] * 255.;
        g_free (temp);
    }
    if ((value = rsvg_property_bag_lookup (atts, "slope"))) {
        data->slope = g_ascii_strtod (value, NULL) * 255.;
    }
    if ((value = rsvg_property_bag_lookup (atts, "intercept"))) {
        data->intercept = g_ascii_strtod (value, NULL) * 255.;
    }
    if ((value = rsvg_property_bag_lookup (atts, "amplitude"))) {
        data->amplitude = g_ascii_strtod (value, NULL) * 255.;
    }
    if ((value = rsvg_property_bag_lookup (atts, "exponent"))) {
        data->exponent = g_ascii_strtod (value, NULL);
    }
    if ((value = rsvg_property_bag_lookup (atts, "offset"))) {
        data->offset = g_ascii_strtod (value, NULL) * 255.;
    }
}