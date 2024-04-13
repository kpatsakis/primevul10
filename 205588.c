rsvg_filter_store_output (GString * name, RsvgFilterPrimitiveOutput result, RsvgFilterContext * ctx)
{
    RsvgFilterPrimitiveOutput *store;

    cairo_surface_destroy (ctx->lastresult.surface);

    store = g_new0 (RsvgFilterPrimitiveOutput, 1);
    *store = result;

    if (name->str[0] != '\0') {
        cairo_surface_reference (result.surface);        /* increments the references for the table */
        g_hash_table_insert (ctx->results, g_strdup (name->str), store);
    }

    cairo_surface_reference (result.surface);    /* increments the references for the last result */
    ctx->lastresult = result;
}