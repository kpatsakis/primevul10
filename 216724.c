multi_uninit(struct multi_context *m)
{
    if (m->thread_mode & MC_WORK_THREAD)
    {
        multi_top_free(m);
        m->thread_mode = MC_UNDEF;
    }
    else if (m->thread_mode)
    {
        if (m->hash)
        {
            struct hash_iterator hi;
            struct hash_element *he;

            hash_iterator_init(m->iter, &hi);
            while ((he = hash_iterator_next(&hi)))
            {
                struct multi_instance *mi = (struct multi_instance *) he->value;
                mi->did_iter = false;
                multi_close_instance(m, mi, true);
            }
            hash_iterator_free(&hi);

            multi_reap_all(m);

            hash_free(m->hash);
            hash_free(m->vhash);
            hash_free(m->iter);
#ifdef MANAGEMENT_DEF_AUTH
            hash_free(m->cid_hash);
#endif
            m->hash = NULL;

            free(m->instances);

#ifdef ENABLE_ASYNC_PUSH
            hash_free(m->inotify_watchers);
            m->inotify_watchers = NULL;
#endif

            schedule_free(m->schedule);
            mbuf_free(m->mbuf);
            ifconfig_pool_free(m->ifconfig_pool);
            frequency_limit_free(m->new_connection_limiter);
            multi_reap_free(m->reaper);
            mroute_helper_free(m->route_helper);
            multi_tcp_free(m->mtcp);
            m->thread_mode = MC_UNDEF;
        }
    }
}