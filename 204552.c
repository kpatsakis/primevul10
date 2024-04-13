_rsvg_node_poly_free (RsvgNode * self)
{
    RsvgNodePoly *poly = (RsvgNodePoly *) self;
    if (poly->path)
        rsvg_cairo_path_destroy (poly->path);
    _rsvg_node_finalize (&poly->super);
    g_free (poly);
}