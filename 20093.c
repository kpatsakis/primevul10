static void flatviews_init(struct uc_struct *uc)
{
    if (uc->flat_views) {
        return;
    }

    uc->flat_views = g_hash_table_new_full(NULL, NULL, NULL,
                                       (GDestroyNotify) flatview_unref);

    if (!uc->empty_view) {
        uc->empty_view = generate_memory_topology(uc, NULL);
        /* We keep it alive forever in the global variable.  */
        flatview_ref(uc->empty_view);
        g_hash_table_replace(uc->flat_views, NULL, uc->empty_view);
    }
}