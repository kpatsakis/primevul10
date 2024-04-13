rsvg_filter_primitive_displacement_map_free (gpointer impl)
{
    RsvgFilterPrimitiveDisplacementMap *dmap = impl;

    g_string_free (dmap->in2, TRUE);

    rsvg_filter_primitive_free (impl);
}