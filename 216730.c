multi_process_incoming_tun(struct multi_context *m, const unsigned int mpp_flags)
{
    struct gc_arena gc = gc_new();
    bool ret = true;

    if (BLEN(&m->top.c2.buf) > 0)
    {
        unsigned int mroute_flags;
        struct mroute_addr src, dest;
        const int dev_type = TUNNEL_TYPE(m->top.c1.tuntap);
        int16_t vid = 0;

#ifdef ENABLE_PF
        struct mroute_addr esrc, *e1, *e2;
        if (dev_type == DEV_TYPE_TUN)
        {
            e1 = NULL;
            e2 = &src;
        }
        else
        {
            e1 = e2 = &esrc;
            mroute_addr_reset(&esrc);
        }
#endif

#ifdef MULTI_DEBUG_EVENT_LOOP
        printf("TUN -> TCP/UDP [%d]\n", BLEN(&m->top.c2.buf));
#endif

        if (m->pending)
        {
            return true;
        }

        if (dev_type == DEV_TYPE_TAP && m->top.options.vlan_tagging)
        {
            vid = vlan_decapsulate(&m->top, &m->top.c2.buf);
            if (vid < 0)
            {
                return false;
            }
        }

        /*
         * Route an incoming tun/tap packet to
         * the appropriate multi_instance object.
         */

        mroute_flags = mroute_extract_addr_from_packet(&src,
                                                       &dest,
#ifdef ENABLE_PF
                                                       e1,
#else
                                                       NULL,
#endif
                                                       NULL,
                                                       vid,
                                                       &m->top.c2.buf,
                                                       dev_type);

        if (mroute_flags & MROUTE_EXTRACT_SUCCEEDED)
        {
            struct context *c;

            /* broadcast or multicast dest addr? */
            if (mroute_flags & (MROUTE_EXTRACT_BCAST|MROUTE_EXTRACT_MCAST))
            {
                /* for now, treat multicast as broadcast */
#ifdef ENABLE_PF
                multi_bcast(m, &m->top.c2.buf, NULL, e2, vid);
#else
                multi_bcast(m, &m->top.c2.buf, NULL, NULL, vid);
#endif
            }
            else
            {
                multi_set_pending(m, multi_get_instance_by_virtual_addr(m, &dest, dev_type == DEV_TYPE_TUN));

                if (m->pending)
                {
                    /* get instance context */
                    c = &m->pending->context;

                    set_prefix(m->pending);

#ifdef ENABLE_PF
                    if (!pf_addr_test(&c->c2.pf, c, e2, "tun_tap_src_addr"))
                    {
                        msg(D_PF_DROPPED, "PF: addr[%s] -> client packet dropped by packet filter",
                            mroute_addr_print_ex(&src, MAPF_SHOW_ARP, &gc));
                        buf_reset_len(&c->c2.buf);
                    }
                    else
#endif
                    {
                        if (multi_output_queue_ready(m, m->pending))
                        {
                            /* transfer packet pointer from top-level context buffer to instance */
                            c->c2.buf = m->top.c2.buf;
                        }
                        else
                        {
                            /* drop packet */
                            msg(D_MULTI_DROPPED, "MULTI: packet dropped due to output saturation (multi_process_incoming_tun)");
                            buf_reset_len(&c->c2.buf);
                        }
                    }

                    /* encrypt in instance context */
                    process_incoming_tun(c);

                    /* postprocess and set wakeup */
                    ret = multi_process_post(m, m->pending, mpp_flags);

                    clear_prefix();
                }
            }
        }
    }
    gc_free(&gc);
    return ret;
}