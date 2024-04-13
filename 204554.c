_rsvg_node_poly_set_atts (RsvgNode * self, RsvgHandle * ctx, RsvgPropertyBag * atts)
{
    RsvgNodePoly *poly = (RsvgNodePoly *) self;
    const char *klazz = NULL, *id = NULL, *value;

    if (rsvg_property_bag_size (atts)) {
        /* support for svg < 1.0 which used verts */
        if ((value = rsvg_property_bag_lookup (atts, "verts"))
            || (value = rsvg_property_bag_lookup (atts, "points"))) {
            if (poly->path)
                rsvg_cairo_path_destroy (poly->path);
            poly->path = _rsvg_node_poly_build_path (value,
                                                     RSVG_NODE_TYPE (self) == RSVG_NODE_TYPE_POLYGON);
        }
        if ((value = rsvg_property_bag_lookup (atts, "class")))
            klazz = value;
        if ((value = rsvg_property_bag_lookup (atts, "id"))) {
            id = value;
            rsvg_defs_register_name (ctx->priv->defs, value, self);
        }

        rsvg_parse_style_attrs (ctx, self->state,
                                RSVG_NODE_TYPE (self) == RSVG_NODE_TYPE_POLYLINE ? "polyline" : "polygon",
                                klazz, id, atts);
    }

}