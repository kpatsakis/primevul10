filter_primitive_set_x_y_width_height_atts (RsvgFilterPrimitive *prim, RsvgPropertyBag *atts)
{
    const char *value;

    if ((value = rsvg_property_bag_lookup (atts, "x"))) {
        prim->x = rsvg_length_parse (value, LENGTH_DIR_HORIZONTAL);
        prim->x_specified = TRUE;
    }
    if ((value = rsvg_property_bag_lookup (atts, "y"))) {
        prim->y = rsvg_length_parse (value, LENGTH_DIR_VERTICAL);
        prim->y_specified = TRUE;
    }
    if ((value = rsvg_property_bag_lookup (atts, "width"))) {
        prim->width = rsvg_length_parse (value, LENGTH_DIR_HORIZONTAL);
        prim->width_specified = TRUE;
    }
    if ((value = rsvg_property_bag_lookup (atts, "height"))) {
        prim->height = rsvg_length_parse (value, LENGTH_DIR_VERTICAL);
        prim->height_specified = TRUE;
    }
}