rsvg_filter_primitive_convolve_matrix_free (gpointer impl)
{
    RsvgFilterPrimitiveConvolveMatrix *convolve = impl;

    g_free (convolve->KernelMatrix);

    rsvg_filter_primitive_free (impl);
}