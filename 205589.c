rsvg_filter_primitive_composite_free (gpointer impl)
{
    RsvgFilterPrimitiveComposite *composite = impl;

    g_string_free (composite->in2, TRUE);

    rsvg_filter_primitive_free (impl);
}