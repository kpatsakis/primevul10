multi_close_instance(struct multi_context *m,
                     struct multi_instance *mi,
                     bool shutdown)
{
    perf_push(PERF_MULTI_CLOSE_INSTANCE);

    ASSERT(!mi->halt);
    mi->halt = true;

    dmsg(D_MULTI_DEBUG, "MULTI: multi_close_instance called");

    /* adjust current client connection count */
    m->n_clients += mi->n_clients_delta;
    update_mstat_n_clients(m->n_clients);
    mi->n_clients_delta = 0;

    /* prevent dangling pointers */
    if (m->pending == mi)
    {
        multi_set_pending(m, NULL);
    }
    if (m->earliest_wakeup == mi)
    {
        m->earliest_wakeup = NULL;
    }

    if (!shutdown)
    {
        if (mi->did_real_hash)
        {
            ASSERT(hash_remove(m->hash, &mi->real));
        }
        if (mi->did_iter)
        {
            ASSERT(hash_remove(m->iter, &mi->real));
        }
#ifdef MANAGEMENT_DEF_AUTH
        if (mi->did_cid_hash)
        {
            ASSERT(hash_remove(m->cid_hash, &mi->context.c2.mda_context.cid));
        }
#endif

#ifdef ENABLE_ASYNC_PUSH
        if (mi->inotify_watch != -1)
        {
            hash_remove(m->inotify_watchers, (void *) (unsigned long)mi->inotify_watch);
            mi->inotify_watch = -1;
        }
#endif

        if (mi->context.c2.tls_multi->peer_id != MAX_PEER_ID)
        {
            m->instances[mi->context.c2.tls_multi->peer_id] = NULL;
        }

        schedule_remove_entry(m->schedule, (struct schedule_entry *) mi);

        ifconfig_pool_release(m->ifconfig_pool, mi->vaddr_handle, false);

        if (mi->did_iroutes)
        {
            multi_del_iroutes(m, mi);
            mi->did_iroutes = false;
        }

        if (m->mtcp)
        {
            multi_tcp_dereference_instance(m->mtcp, mi);
        }

        mbuf_dereference_instance(m->mbuf, mi);
    }

#ifdef MANAGEMENT_DEF_AUTH
    set_cc_config(mi, NULL);
#endif

    multi_client_disconnect_script(mi);

    if (mi->did_open_context)
    {
        close_context(&mi->context, SIGTERM, CC_GC_FREE);
    }

    multi_tcp_instance_specific_free(mi);

    ungenerate_prefix(mi);

    /*
     * Don't actually delete the instance memory allocation yet,
     * because virtual routes may still point to it.  Let the
     * vhash reaper deal with it.
     */
    multi_instance_dec_refcount(mi);

    perf_pop();
}