multi_delete_dup(struct multi_context *m, struct multi_instance *new_mi)
{
    if (new_mi)
    {
        const char *new_cn = tls_common_name(new_mi->context.c2.tls_multi, true);
        if (new_cn)
        {
            struct hash_iterator hi;
            struct hash_element *he;
            int count = 0;

            hash_iterator_init(m->iter, &hi);
            while ((he = hash_iterator_next(&hi)))
            {
                struct multi_instance *mi = (struct multi_instance *) he->value;
                if (mi != new_mi && !mi->halt)
                {
                    const char *cn = tls_common_name(mi->context.c2.tls_multi, true);
                    if (cn && !strcmp(cn, new_cn))
                    {
                        mi->did_iter = false;
                        multi_close_instance(m, mi, false);
                        hash_iterator_delete_element(&hi);
                        ++count;
                    }
                }
            }
            hash_iterator_free(&hi);

            if (count)
            {
                msg(D_MULTI_LOW, "MULTI: new connection by client '%s' will cause previous active sessions by this client to be dropped.  Remember to use the --duplicate-cn option if you want multiple clients using the same certificate or username to concurrently connect.", new_cn);
            }
        }
    }
}