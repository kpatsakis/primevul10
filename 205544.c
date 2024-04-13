rsvg_filter_primitive_color_matrix_set_atts (RsvgNode *node, gpointer impl, RsvgHandle *handle, RsvgPropertyBag *atts)
{
    RsvgFilterPrimitiveColorMatrix *filter = impl;
    gint type;
    gsize listlen = 0;
    const char *value;

    type = 0;

    if ((value = rsvg_property_bag_lookup (atts, "in")))
        g_string_assign (filter->super.in, value);
    if ((value = rsvg_property_bag_lookup (atts, "result")))
        g_string_assign (filter->super.result, value);

    filter_primitive_set_x_y_width_height_atts ((RsvgFilterPrimitive *) filter, atts);

    if ((value = rsvg_property_bag_lookup (atts, "values"))) {
        unsigned int i;
        double *temp;
        if (!rsvg_css_parse_number_list (value,
                                         NUMBER_LIST_LENGTH_MAXIMUM,
                                         20,
                                         &temp,
                                         &listlen)) {
            rsvg_node_set_attribute_parse_error (node, "values", "invalid number list");
            return;
        }

        filter->KernelMatrix = g_new0 (int, listlen);
        for (i = 0; i < listlen; i++)
            filter->KernelMatrix[i] = temp[i] * 255.;
        g_free (temp);
    }
    if ((value = rsvg_property_bag_lookup (atts, "type"))) {
        if (!strcmp (value, "matrix"))
            type = 0;
        else if (!strcmp (value, "saturate"))
            type = 1;
        else if (!strcmp (value, "hueRotate"))
            type = 2;
        else if (!strcmp (value, "luminanceToAlpha"))
            type = 3;
        else
            type = 0;
    }

    if (type == 0) {
        if (listlen != 20) {
            if (filter->KernelMatrix != NULL)
                g_free (filter->KernelMatrix);
            filter->KernelMatrix = g_new0 (int, 20);
        }
    } else if (type == 1) {
        float s;
        if (listlen != 0) {
            s = filter->KernelMatrix[0];
            g_free (filter->KernelMatrix);
        } else
            s = 255;
        filter->KernelMatrix = g_new0 (int, 20);

        filter->KernelMatrix[0] = 0.213 * 255. + 0.787 * s;
        filter->KernelMatrix[1] = 0.715 * 255. - 0.715 * s;
        filter->KernelMatrix[2] = 0.072 * 255. - 0.072 * s;
        filter->KernelMatrix[5] = 0.213 * 255. - 0.213 * s;
        filter->KernelMatrix[6] = 0.715 * 255. + 0.285 * s;
        filter->KernelMatrix[7] = 0.072 * 255. - 0.072 * s;
        filter->KernelMatrix[10] = 0.213 * 255. - 0.213 * s;
        filter->KernelMatrix[11] = 0.715 * 255. - 0.715 * s;
        filter->KernelMatrix[12] = 0.072 * 255. + 0.928 * s;
        filter->KernelMatrix[18] = 255;
    } else if (type == 2) {
        double cosval, sinval, arg;

        if (listlen != 0) {
            arg = (double) filter->KernelMatrix[0] / 255.;
            g_free (filter->KernelMatrix);
        } else
            arg = 0;

        cosval = cos (arg);
        sinval = sin (arg);

        filter->KernelMatrix = g_new0 (int, 20);

        filter->KernelMatrix[0] = (0.213 + cosval * 0.787 + sinval * -0.213) * 255.;
        filter->KernelMatrix[1] = (0.715 + cosval * -0.715 + sinval * -0.715) * 255.;
        filter->KernelMatrix[2] = (0.072 + cosval * -0.072 + sinval * 0.928) * 255.;
        filter->KernelMatrix[5] = (0.213 + cosval * -0.213 + sinval * 0.143) * 255.;
        filter->KernelMatrix[6] = (0.715 + cosval * 0.285 + sinval * 0.140) * 255.;
        filter->KernelMatrix[7] = (0.072 + cosval * -0.072 + sinval * -0.283) * 255.;
        filter->KernelMatrix[10] = (0.213 + cosval * -0.213 + sinval * -0.787) * 255.;
        filter->KernelMatrix[11] = (0.715 + cosval * -0.715 + sinval * 0.715) * 255.;
        filter->KernelMatrix[12] = (0.072 + cosval * 0.928 + sinval * 0.072) * 255.;
        filter->KernelMatrix[18] = 255;
    } else if (type == 3) {
        if (filter->KernelMatrix != NULL)
            g_free (filter->KernelMatrix);

        filter->KernelMatrix = g_new0 (int, 20);

        filter->KernelMatrix[15] = 0.2125 * 255.;
        filter->KernelMatrix[16] = 0.7154 * 255.;
        filter->KernelMatrix[17] = 0.0721 * 255.;
    } else {
        g_assert_not_reached ();
    }
}