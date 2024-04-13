management_delete_event(void *arg, event_t event)
{
    struct multi_context *m = (struct multi_context *) arg;
    if (m->mtcp)
    {
        multi_tcp_delete_event(m->mtcp, event);
    }
}