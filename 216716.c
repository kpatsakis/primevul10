multi_process_post(struct multi_context *m, struct multi_instance *mi, const unsigned int flags)
{
    bool ret = true;

    if (!IS_SIG(&mi->context) && ((flags & MPP_PRE_SELECT) || ((flags & MPP_CONDITIONAL_PRE_SELECT) && !ANY_OUT(&mi->context))))
    {
#if defined(ENABLE_ASYNC_PUSH) && defined(ENABLE_DEF_AUTH)
        bool was_authenticated = false;
        struct key_state *ks = NULL;
        if (mi->context.c2.tls_multi)
        {
            ks = &mi->context.c2.tls_multi->session[TM_ACTIVE].key[KS_PRIMARY];
            was_authenticated = ks->authenticated;
        }
#endif

        /* figure timeouts and fetch possible outgoing
         * to_link packets (such as ping or TLS control) */
        pre_select(&mi->context);

#if defined(ENABLE_ASYNC_PUSH) && defined(ENABLE_DEF_AUTH)
        if (ks && ks->auth_control_file && ks->auth_deferred && !was_authenticated)
        {
            /* watch acf file */
            long watch_descriptor = inotify_add_watch(m->top.c2.inotify_fd, ks->auth_control_file, IN_CLOSE_WRITE | IN_ONESHOT);
            if (watch_descriptor >= 0)
            {
                if (mi->inotify_watch != -1)
                {
                    hash_remove(m->inotify_watchers, (void *) (unsigned long)mi->inotify_watch);
                }
                hash_add(m->inotify_watchers, (const uintptr_t *)watch_descriptor, mi, true);
                mi->inotify_watch = watch_descriptor;
            }
            else
            {
                msg(M_NONFATAL | M_ERRNO, "MULTI: inotify_add_watch error");
            }
        }
#endif

        if (!IS_SIG(&mi->context))
        {
            /* connection is "established" when SSL/TLS key negotiation succeeds
             * and (if specified) auth user/pass succeeds */
            if (!mi->connection_established_flag && CONNECTION_ESTABLISHED(&mi->context))
            {
                multi_connection_established(m, mi);
            }

            /* tell scheduler to wake us up at some point in the future */
            multi_schedule_context_wakeup(m, mi);
        }
    }

    if (IS_SIG(&mi->context))
    {
        if (flags & MPP_CLOSE_ON_SIGNAL)
        {
            multi_close_instance_on_signal(m, mi);
            ret = false;
        }
    }
    else
    {
        /* continue to pend on output? */
        multi_set_pending(m, ANY_OUT(&mi->context) ? mi : NULL);

#ifdef MULTI_DEBUG_EVENT_LOOP
        printf("POST %s[%d] to=%d lo=%d/%d w=%" PRIi64 "/%ld\n",
               id(mi),
               (int) (mi == m->pending),
               mi ? mi->context.c2.to_tun.len : -1,
               mi ? mi->context.c2.to_link.len : -1,
               (mi && mi->context.c2.fragment) ? mi->context.c2.fragment->outgoing.len : -1,
               (int64_t)mi->context.c2.timeval.tv_sec,
               (long)mi->context.c2.timeval.tv_usec);
#endif
    }

    if ((flags & MPP_RECORD_TOUCH) && m->mpp_touched)
    {
        *m->mpp_touched = mi;
    }

    return ret;
}