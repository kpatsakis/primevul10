multi_process_signal(struct multi_context *m)
{
    if (m->top.sig->signal_received == SIGUSR2)
    {
        struct status_output *so = status_open(NULL, 0, M_INFO, NULL, 0);
        multi_print_status(m, so, m->status_file_version);
        status_close(so);
        m->top.sig->signal_received = 0;
        return false;
    }
    else if (proto_is_dgram(m->top.options.ce.proto)
             && is_exit_restart(m->top.sig->signal_received)
             && (m->deferred_shutdown_signal.signal_received == 0)
             && m->top.options.ce.explicit_exit_notification != 0)
    {
        multi_push_restart_schedule_exit(m, m->top.options.ce.explicit_exit_notification == 2);
        return false;
    }
    return true;
}