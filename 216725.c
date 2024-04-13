multi_schedule_context_wakeup(struct multi_context *m, struct multi_instance *mi)
{
    /* calculate an absolute wakeup time */
    ASSERT(!openvpn_gettimeofday(&mi->wakeup, NULL));
    tv_add(&mi->wakeup, &mi->context.c2.timeval);

    /* tell scheduler to wake us up at some point in the future */
    schedule_add_entry(m->schedule,
                       (struct schedule_entry *) mi,
                       &mi->wakeup,
                       compute_wakeup_sigma(&mi->context.c2.timeval));
}