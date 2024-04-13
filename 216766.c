multi_process_drop_outgoing_tun(struct multi_context *m, const unsigned int mpp_flags)
{
    struct multi_instance *mi = m->pending;

    ASSERT(mi);

    set_prefix(mi);

    msg(D_MULTI_ERRORS, "MULTI: Outgoing TUN queue full, dropped packet len=%d",
        mi->context.c2.to_tun.len);

    buf_reset(&mi->context.c2.to_tun);

    multi_process_post(m, mi, mpp_flags);
    clear_prefix();
}