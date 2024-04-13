rsvg_filter_context_free (RsvgFilterContext * ctx)
{
    if (!ctx)
        return;

    if (ctx->bg_surface)
        cairo_surface_destroy (ctx->bg_surface);

    g_free (ctx);
}