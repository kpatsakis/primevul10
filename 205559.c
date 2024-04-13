rsvg_filter_primitive_convolve_matrix_set_atts (RsvgNode *node, gpointer impl, RsvgHandle *handle, RsvgPropertyBag *atts)
{
    RsvgFilterPrimitiveConvolveMatrix *filter = impl;
    gint i, j;
    const char *value;
    gboolean has_target_x, has_target_y;

    has_target_x = 0;
    has_target_y = 0;

    if ((value = rsvg_property_bag_lookup (atts, "in")))
        g_string_assign (filter->super.in, value);
    if ((value = rsvg_property_bag_lookup (atts, "result")))
        g_string_assign (filter->super.result, value);

    filter_primitive_set_x_y_width_height_atts ((RsvgFilterPrimitive *) filter, atts);

    if ((value = rsvg_property_bag_lookup (atts, "targetX"))) {
        has_target_x = 1;
        filter->targetx = atoi (value);
    }
    if ((value = rsvg_property_bag_lookup (atts, "targetY"))) {
        has_target_y = 1;
        filter->targety = atoi (value);
    }
    if ((value = rsvg_property_bag_lookup (atts, "bias")))
        filter->bias = atof (value);
    if ((value = rsvg_property_bag_lookup (atts, "preserveAlpha"))) {
        if (!strcmp (value, "true"))
            filter->preservealpha = TRUE;
        else
            filter->preservealpha = FALSE;
    }
    if ((value = rsvg_property_bag_lookup (atts, "divisor")))
        filter->divisor = atof (value);

    if ((value = rsvg_property_bag_lookup (atts, "order"))) {
        double tempx, tempy;
        if (rsvg_css_parse_number_optional_number (value, &tempx, &tempy)
            && tempx >= 1.0 && tempy <= 100.0
            && tempy >= 1.0 && tempy <= 100.0) {
            filter->orderx = (int) tempx;
            filter->ordery = (int) tempy;
            g_assert (filter->orderx >= 1);
            g_assert (filter->ordery >= 1);

#define SIZE_OVERFLOWS(a,b) (G_UNLIKELY ((b) > 0 && (a) > G_MAXSIZE / (b)))

            if (SIZE_OVERFLOWS (filter->orderx, filter->ordery)) {
                rsvg_node_set_attribute_parse_error (node, "order", "number of kernelMatrix elements would be too big");
                return;
            }
        } else {
            rsvg_node_set_attribute_parse_error (node, "order", "invalid size for convolve matrix");
            return;
        }
    }

    if ((value = rsvg_property_bag_lookup (atts, "kernelUnitLength"))) {
        if (!rsvg_css_parse_number_optional_number (value, &filter->dx, &filter->dy)) {
            rsvg_node_set_attribute_parse_error (node, "kernelUnitLength", "expected number-optional-number");
            return;
        }
    }

    if ((value = rsvg_property_bag_lookup (atts, "kernelMatrix"))) {
        gsize num_elems;
        gsize got_num_elems;

        num_elems = filter->orderx * filter->ordery;

        if (!rsvg_css_parse_number_list (value,
                                         NUMBER_LIST_LENGTH_EXACT,
                                         num_elems,
                                         &filter->KernelMatrix,
                                         &got_num_elems)) {
            rsvg_node_set_attribute_parse_error (node, "kernelMatrix", "expected a matrix of numbers");
            return;
        }

        g_assert (num_elems == got_num_elems);
    }

    if ((value = rsvg_property_bag_lookup (atts, "edgeMode"))) {
        if (!strcmp (value, "duplicate")) {
            filter->edgemode = EDGE_MODE_DUPLICATE;
        } else if (!strcmp (value, "wrap")) {
            filter->edgemode = EDGE_MODE_WRAP;
        } else if (!strcmp (value, "none")) {
            filter->edgemode = EDGE_MODE_NONE;
        } else {
            rsvg_node_set_attribute_parse_error (node, "edgeMode", "expected 'duplicate' | 'wrap' | 'none'");
            return;
        }
    }

    if (filter->divisor == 0) {
        for (j = 0; j < filter->orderx; j++)
            for (i = 0; i < filter->ordery; i++)
                filter->divisor += filter->KernelMatrix[j + i * filter->orderx];
    }

    if (filter->divisor == 0)
        filter->divisor = 1;

    if (!has_target_x) {
        filter->targetx = floor (filter->orderx / 2);
    }
    if (!has_target_y) {
        filter->targety = floor (filter->ordery / 2);
    }
}