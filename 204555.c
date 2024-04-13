rsvg_node_path_free (RsvgNode * self)
{
    RsvgNodePath *path = (RsvgNodePath *) self;
    if (path->path)
        rsvg_cairo_path_destroy (path->path);
    _rsvg_node_finalize (&path->super);
    g_free (path);
}