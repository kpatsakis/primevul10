rsvg_filter_primitive_color_matrix_free (gpointer impl)
{
    RsvgFilterPrimitiveColorMatrix *matrix = impl;

    g_free (matrix->KernelMatrix);

    rsvg_filter_primitive_free (impl);
}