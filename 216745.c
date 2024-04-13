multi_create_instance(struct multi_context *m, const struct mroute_addr *real)
{
    struct gc_arena gc = gc_new();
    struct multi_instance *mi;

    perf_push(PERF_MULTI_CREATE_INSTANCE);

    msg(D_MULTI_MEDIUM, "MULTI: multi_create_instance called");

    ALLOC_OBJ_CLEAR(mi, struct multi_instance);

    mi->gc = gc_new();
    multi_instance_inc_refcount(mi);
    mi->vaddr_handle = -1;
    mi->created = now;
    mroute_addr_init(&mi->real);

    if (real)
    {
        mi->real = *real;
        generate_prefix(mi);
    }

    mi->did_open_context = true;
    inherit_context_child(&mi->context, &m->top);
    if (IS_SIG(&mi->context))
    {
        goto err;
    }

    mi->context.c2.context_auth = CAS_PENDING;

    if (hash_n_elements(m->hash) >= m->max_clients)
    {
        msg(D_MULTI_ERRORS, "MULTI: new incoming connection would exceed maximum number of clients (%d)", m->max_clients);
        goto err;
    }

    if (!real) /* TCP mode? */
    {
        if (!multi_tcp_instance_specific_init(m, mi))
        {
            goto err;
        }
        generate_prefix(mi);
    }

    if (!hash_add(m->iter, &mi->real, mi, false))
    {
        msg(D_MULTI_LOW, "MULTI: unable to add real address [%s] to iterator hash table",
            mroute_addr_print(&mi->real, &gc));
        goto err;
    }
    mi->did_iter = true;

#ifdef MANAGEMENT_DEF_AUTH
    do
    {
        mi->context.c2.mda_context.cid = m->cid_counter++;
    } while (!hash_add(m->cid_hash, &mi->context.c2.mda_context.cid, mi, false));
    mi->did_cid_hash = true;
#endif

    mi->context.c2.push_reply_deferred = true;

#ifdef ENABLE_ASYNC_PUSH
    mi->context.c2.push_request_received = false;
    mi->inotify_watch = -1;
#endif

    if (!multi_process_post(m, mi, MPP_PRE_SELECT))
    {
        msg(D_MULTI_ERRORS, "MULTI: signal occurred during client instance initialization");
        goto err;
    }

    perf_pop();
    gc_free(&gc);
    return mi;

err:
    multi_close_instance(m, mi, false);
    perf_pop();
    gc_free(&gc);
    return NULL;
}