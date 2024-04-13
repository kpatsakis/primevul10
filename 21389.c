static void vnc_tight_cleanup(Notifier *n, void *value)
{
    g_free(color_count_palette);
    color_count_palette = NULL;
}