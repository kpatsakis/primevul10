multi_process_timeout(struct multi_context *m, const unsigned int mpp_flags)
{
    bool ret = true;

#ifdef MULTI_DEBUG_EVENT_LOOP
    printf("%s -> TIMEOUT\n", id(m->earliest_wakeup));
#endif

    /* instance marked for wakeup? */
    if (m->earliest_wakeup)
    {
        if (m->earliest_wakeup == (struct multi_instance *)&m->deferred_shutdown_signal)
        {
            schedule_remove_entry(m->schedule, (struct schedule_entry *) &m->deferred_shutdown_signal);
            throw_signal(m->deferred_shutdown_signal.signal_received);
        }
        else
        {
            set_prefix(m->earliest_wakeup);
            ret = multi_process_post(m, m->earliest_wakeup, mpp_flags);
            clear_prefix();
        }
        m->earliest_wakeup = NULL;
    }
    return ret;
}