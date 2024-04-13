rsvg_filter_primitive_specular_lighting_set_atts (RsvgNode *node, gpointer impl, RsvgHandle *handle, RsvgPropertyBag *atts)
{
    RsvgFilterPrimitiveSpecularLighting *filter = impl;
    const char *value;

    if ((value = rsvg_property_bag_lookup (atts, "in")))
        g_string_assign (filter->super.in, value);
    if ((value = rsvg_property_bag_lookup (atts, "result")))
        g_string_assign (filter->super.result, value);

    filter_primitive_set_x_y_width_height_atts ((RsvgFilterPrimitive *) filter, atts);

    if ((value = rsvg_property_bag_lookup (atts, "lighting-color"))) {
        RsvgCssColorSpec spec;
        RsvgState *state;

        spec = rsvg_css_parse_color (value, ALLOW_INHERIT_YES, ALLOW_CURRENT_COLOR_YES);

        switch (spec.kind) {
        case RSVG_CSS_COLOR_SPEC_INHERIT:
            /* FIXME: we should inherit; see how stop-color is handled in rsvg-styles.c */
            break;

        case RSVG_CSS_COLOR_SPEC_CURRENT_COLOR:
            state = rsvg_state_new ();
            rsvg_state_reconstruct (state, node);
            filter->lightingcolor = state->current_color;
            break;

        case RSVG_CSS_COLOR_SPEC_ARGB:
            filter->lightingcolor = spec.argb;
            break;

        case RSVG_CSS_COLOR_PARSE_ERROR:
            rsvg_node_set_attribute_parse_error (node, "lighting-color", "Invalid color");
            break;

        default:
            g_assert_not_reached ();
        }
    }

    if ((value = rsvg_property_bag_lookup (atts, "specularConstant")))
        filter->specularConstant = g_ascii_strtod (value, NULL);
    if ((value = rsvg_property_bag_lookup (atts, "specularExponent")))
        filter->specularExponent = g_ascii_strtod (value, NULL);
    if ((value = rsvg_property_bag_lookup (atts, "surfaceScale")))
        filter->surfaceScale = g_ascii_strtod (value, NULL);
}