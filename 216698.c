management_kill_by_cid(void *arg, const unsigned long cid, const char *kill_msg)
{
    struct multi_context *m = (struct multi_context *) arg;
    struct multi_instance *mi = lookup_by_cid(m, cid);
    if (mi)
    {
        send_restart(&mi->context, kill_msg); /* was: multi_signal_instance (m, mi, SIGTERM); */
        multi_schedule_context_wakeup(m, mi);
        return true;
    }
    else
    {
        return false;
    }
}