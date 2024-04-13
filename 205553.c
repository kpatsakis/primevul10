rsvg_filter_primitive_image_free (gpointer impl)
{
    RsvgFilterPrimitiveImage *image = impl;

    if (image->href)
        g_string_free (image->href, TRUE);

    rsvg_filter_primitive_free (impl);
}