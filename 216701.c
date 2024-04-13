multi_process_file_closed(struct multi_context *m, const unsigned int mpp_flags)
{
    char buffer[INOTIFY_EVENT_BUFFER_SIZE];
    size_t buffer_i = 0;
    int r = read(m->top.c2.inotify_fd, buffer, INOTIFY_EVENT_BUFFER_SIZE);

    while (buffer_i < r)
    {
        /* parse inotify events */
        struct inotify_event *pevent = (struct inotify_event *) &buffer[buffer_i];
        size_t event_size = sizeof(struct inotify_event) + pevent->len;
        buffer_i += event_size;

        msg(D_MULTI_DEBUG, "MULTI: modified fd %d, mask %d", pevent->wd, pevent->mask);

        struct multi_instance *mi = hash_lookup(m->inotify_watchers, (void *) (unsigned long) pevent->wd);

        if (pevent->mask & IN_CLOSE_WRITE)
        {
            if (mi)
            {
                /* continue authentication, perform NCP negotiation and send push_reply */
                multi_process_post(m, mi, mpp_flags);

                /* With NCP and deferred authentication, we perform cipher negotiation and
                 * data channel keys generation on incoming push request, assuming that auth
                 * succeeded. When auth succeeds in between push requests and async push is used,
                 * we send push reply immediately. Above multi_process_post() call performs
                 * NCP negotiation and here we do keys generation. */

                struct context *c = &mi->context;
                struct frame *frame_fragment = NULL;
#ifdef ENABLE_FRAGMENT
                if (c->options.ce.fragment)
                {
                    frame_fragment = &c->c2.frame_fragment;
                }
#endif
                struct tls_session *session = &c->c2.tls_multi->session[TM_ACTIVE];
                if (!tls_session_update_crypto_params(session, &c->options,
                                                      &c->c2.frame, frame_fragment))
                {
                    msg(D_TLS_ERRORS, "TLS Error: initializing data channel failed");
                    register_signal(c, SIGUSR1, "init-data-channel-failed");
                }
            }
            else
            {
                msg(D_MULTI_ERRORS, "MULTI: multi_instance not found!");
            }
        }
        else if (pevent->mask & IN_IGNORED)
        {
            /* this event is _always_ fired when watch is removed or file is deleted */
            if (mi)
            {
                hash_remove(m->inotify_watchers, (void *) (unsigned long) pevent->wd);
                mi->inotify_watch = -1;
            }
        }
        else
        {
            msg(D_MULTI_ERRORS, "MULTI: unknown mask %d", pevent->mask);
        }
    }
}