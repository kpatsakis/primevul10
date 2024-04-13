rsvg_filter_free (gpointer impl)
{
    RsvgFilter *filter = impl;

    g_free (filter);
}