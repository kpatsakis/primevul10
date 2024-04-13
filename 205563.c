rsvg_filter_get_result (GString * name, RsvgFilterContext * ctx)
{
    RsvgFilterPrimitiveOutput output;
    RsvgFilterPrimitiveOutput *outputpointer;
    output.bounds.x0 = output.bounds.x1 = output.bounds.y0 = output.bounds.y1 = 0;

    if (!strcmp (name->str, "SourceGraphic")) {
        output.surface = cairo_surface_reference (ctx->source_surface);
        return output;
    } else if (!strcmp (name->str, "BackgroundImage")) {
        output.surface = rsvg_filter_get_bg (ctx);
        if (output.surface)
            cairo_surface_reference (output.surface);
        return output;
    } else if (!strcmp (name->str, "") || !strcmp (name->str, "none")) {
        output = ctx->lastresult;
        cairo_surface_reference (output.surface);
        return output;
    } else if (!strcmp (name->str, "SourceAlpha")) {
        output.surface = surface_get_alpha (ctx->source_surface, ctx);
        return output;
    } else if (!strcmp (name->str, "BackgroundAlpha")) {
        output.surface = surface_get_alpha (rsvg_filter_get_bg (ctx), ctx);
        return output;
    }

    outputpointer = (RsvgFilterPrimitiveOutput *) (g_hash_table_lookup (ctx->results, name->str));

    if (outputpointer != NULL) {
        output = *outputpointer;
        cairo_surface_reference (output.surface);
        return output;
    }

    /* g_warning (_("%s not found\n"), name->str); */

    output = ctx->lastresult;
    cairo_surface_reference (output.surface);
    return output;
}