rsvg_filter_primitive_free (gpointer impl)
{
    RsvgFilterPrimitive *primitive = impl;

    g_string_free (primitive->in, TRUE);
    g_string_free (primitive->result, TRUE);

    g_free (primitive);
}