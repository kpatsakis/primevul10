rsvg_filter_primitive_blend_free (gpointer impl)
{
    RsvgFilterPrimitiveBlend *blend = impl;

    g_string_free (blend->in2, TRUE);

    rsvg_filter_primitive_free (impl);
}