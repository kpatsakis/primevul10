management_callback_kill_by_cn(void *arg, const char *del_cn)
{
    struct multi_context *m = (struct multi_context *) arg;
    struct hash_iterator hi;
    struct hash_element *he;
    int count = 0;

    hash_iterator_init(m->iter, &hi);
    while ((he = hash_iterator_next(&hi)))
    {
        struct multi_instance *mi = (struct multi_instance *) he->value;
        if (!mi->halt)
        {
            const char *cn = tls_common_name(mi->context.c2.tls_multi, false);
            if (cn && !strcmp(cn, del_cn))
            {
                multi_signal_instance(m, mi, SIGTERM);
                ++count;
            }
        }
    }
    hash_iterator_free(&hi);
    return count;
}