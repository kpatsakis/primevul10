multi_push_restart_schedule_exit(struct multi_context *m, bool next_server)
{
    struct hash_iterator hi;
    struct hash_element *he;
    struct timeval tv;

    /* tell all clients to restart */
    hash_iterator_init(m->iter, &hi);
    while ((he = hash_iterator_next(&hi)))
    {
        struct multi_instance *mi = (struct multi_instance *) he->value;
        if (!mi->halt)
        {
            send_control_channel_string(&mi->context, next_server ? "RESTART,[N]" : "RESTART", D_PUSH);
            multi_schedule_context_wakeup(m, mi);
        }
    }
    hash_iterator_free(&hi);

    /* reschedule signal */
    ASSERT(!openvpn_gettimeofday(&m->deferred_shutdown_signal.wakeup, NULL));
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    tv_add(&m->deferred_shutdown_signal.wakeup, &tv);

    m->deferred_shutdown_signal.signal_received = m->top.sig->signal_received;

    schedule_add_entry(m->schedule,
                       (struct schedule_entry *) &m->deferred_shutdown_signal,
                       &m->deferred_shutdown_signal.wakeup,
                       compute_wakeup_sigma(&m->deferred_shutdown_signal.wakeup));

    m->top.sig->signal_received = 0;
}