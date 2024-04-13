multi_print_status(struct multi_context *m, struct status_output *so, const int version)
{
    if (m->hash)
    {
        struct gc_arena gc_top = gc_new();
        struct hash_iterator hi;
        const struct hash_element *he;

        status_reset(so);

        if (version == 1) /* WAS: m->status_file_version */
        {
            /*
             * Status file version 1
             */
            status_printf(so, "OpenVPN CLIENT LIST");
            status_printf(so, "Updated,%s", time_string(0, 0, false, &gc_top));
            status_printf(so, "Common Name,Real Address,Bytes Received,Bytes Sent,Connected Since");
            hash_iterator_init(m->hash, &hi);
            while ((he = hash_iterator_next(&hi)))
            {
                struct gc_arena gc = gc_new();
                const struct multi_instance *mi = (struct multi_instance *) he->value;

                if (!mi->halt)
                {
                    status_printf(so, "%s,%s," counter_format "," counter_format ",%s",
                                  tls_common_name(mi->context.c2.tls_multi, false),
                                  mroute_addr_print(&mi->real, &gc),
                                  mi->context.c2.link_read_bytes,
                                  mi->context.c2.link_write_bytes,
                                  time_string(mi->created, 0, false, &gc));
                }
                gc_free(&gc);
            }
            hash_iterator_free(&hi);

            status_printf(so, "ROUTING TABLE");
            status_printf(so, "Virtual Address,Common Name,Real Address,Last Ref");
            hash_iterator_init(m->vhash, &hi);
            while ((he = hash_iterator_next(&hi)))
            {
                struct gc_arena gc = gc_new();
                const struct multi_route *route = (struct multi_route *) he->value;

                if (multi_route_defined(m, route))
                {
                    const struct multi_instance *mi = route->instance;
                    const struct mroute_addr *ma = &route->addr;
                    char flags[2] = {0, 0};

                    if (route->flags & MULTI_ROUTE_CACHE)
                    {
                        flags[0] = 'C';
                    }
                    status_printf(so, "%s%s,%s,%s,%s",
                                  mroute_addr_print(ma, &gc),
                                  flags,
                                  tls_common_name(mi->context.c2.tls_multi, false),
                                  mroute_addr_print(&mi->real, &gc),
                                  time_string(route->last_reference, 0, false, &gc));
                }
                gc_free(&gc);
            }
            hash_iterator_free(&hi);

            status_printf(so, "GLOBAL STATS");
            if (m->mbuf)
            {
                status_printf(so, "Max bcast/mcast queue length,%d",
                              mbuf_maximum_queued(m->mbuf));
            }

            status_printf(so, "END");
        }
        else if (version == 2 || version == 3)
        {
            const char sep = (version == 3) ? '\t' : ',';

            /*
             * Status file version 2 and 3
             */
            status_printf(so, "TITLE%c%s", sep, title_string);
            status_printf(so, "TIME%c%s%c%u", sep, time_string(now, 0, false, &gc_top), sep, (unsigned int)now);
            status_printf(so, "HEADER%cCLIENT_LIST%cCommon Name%cReal Address%cVirtual Address%cVirtual IPv6 Address%cBytes Received%cBytes Sent%cConnected Since%cConnected Since (time_t)%cUsername%cClient ID%cPeer ID%cData Channel Cipher",
                          sep, sep, sep, sep, sep, sep, sep, sep, sep, sep, sep, sep, sep);
            hash_iterator_init(m->hash, &hi);
            while ((he = hash_iterator_next(&hi)))
            {
                struct gc_arena gc = gc_new();
                const struct multi_instance *mi = (struct multi_instance *) he->value;

                if (!mi->halt)
                {
                    status_printf(so, "CLIENT_LIST%c%s%c%s%c%s%c%s%c" counter_format "%c" counter_format "%c%s%c%u%c%s%c"
#ifdef MANAGEMENT_DEF_AUTH
                                  "%lu"
#else
                                  ""
#endif
                                  "%c%" PRIu32 "%c%s",
                                  sep, tls_common_name(mi->context.c2.tls_multi, false),
                                  sep, mroute_addr_print(&mi->real, &gc),
                                  sep, print_in_addr_t(mi->reporting_addr, IA_EMPTY_IF_UNDEF, &gc),
                                  sep, print_in6_addr(mi->reporting_addr_ipv6, IA_EMPTY_IF_UNDEF, &gc),
                                  sep, mi->context.c2.link_read_bytes,
                                  sep, mi->context.c2.link_write_bytes,
                                  sep, time_string(mi->created, 0, false, &gc),
                                  sep, (unsigned int)mi->created,
                                  sep, tls_username(mi->context.c2.tls_multi, false),
#ifdef MANAGEMENT_DEF_AUTH
                                  sep, mi->context.c2.mda_context.cid,
#else
                                  sep,
#endif
                                  sep, mi->context.c2.tls_multi ? mi->context.c2.tls_multi->peer_id : UINT32_MAX,
                                  sep, translate_cipher_name_to_openvpn(mi->context.options.ciphername));
                }
                gc_free(&gc);
            }
            hash_iterator_free(&hi);

            status_printf(so, "HEADER%cROUTING_TABLE%cVirtual Address%cCommon Name%cReal Address%cLast Ref%cLast Ref (time_t)",
                          sep, sep, sep, sep, sep, sep);
            hash_iterator_init(m->vhash, &hi);
            while ((he = hash_iterator_next(&hi)))
            {
                struct gc_arena gc = gc_new();
                const struct multi_route *route = (struct multi_route *) he->value;

                if (multi_route_defined(m, route))
                {
                    const struct multi_instance *mi = route->instance;
                    const struct mroute_addr *ma = &route->addr;
                    char flags[2] = {0, 0};

                    if (route->flags & MULTI_ROUTE_CACHE)
                    {
                        flags[0] = 'C';
                    }
                    status_printf(so, "ROUTING_TABLE%c%s%s%c%s%c%s%c%s%c%u",
                                  sep, mroute_addr_print(ma, &gc), flags,
                                  sep, tls_common_name(mi->context.c2.tls_multi, false),
                                  sep, mroute_addr_print(&mi->real, &gc),
                                  sep, time_string(route->last_reference, 0, false, &gc),
                                  sep, (unsigned int)route->last_reference);
                }
                gc_free(&gc);
            }
            hash_iterator_free(&hi);

            if (m->mbuf)
            {
                status_printf(so, "GLOBAL_STATS%cMax bcast/mcast queue length%c%d",
                              sep, sep, mbuf_maximum_queued(m->mbuf));
            }

            status_printf(so, "END");
        }
        else
        {
            status_printf(so, "ERROR: bad status format version number");
        }

#ifdef PACKET_TRUNCATION_CHECK
        {
            status_printf(so, "HEADER,ERRORS,Common Name,TUN Read Trunc,TUN Write Trunc,Pre-encrypt Trunc,Post-decrypt Trunc");
            hash_iterator_init(m->hash, &hi);
            while ((he = hash_iterator_next(&hi)))
            {
                struct gc_arena gc = gc_new();
                const struct multi_instance *mi = (struct multi_instance *) he->value;

                if (!mi->halt)
                {
                    status_printf(so, "ERRORS,%s," counter_format "," counter_format "," counter_format "," counter_format,
                                  tls_common_name(mi->context.c2.tls_multi, false),
                                  m->top.c2.n_trunc_tun_read,
                                  mi->context.c2.n_trunc_tun_write,
                                  mi->context.c2.n_trunc_pre_encrypt,
                                  mi->context.c2.n_trunc_post_decrypt);
                }
                gc_free(&gc);
            }
            hash_iterator_free(&hi);
        }
#endif /* ifdef PACKET_TRUNCATION_CHECK */

        status_flush(so);
        gc_free(&gc_top);
    }

#ifdef ENABLE_ASYNC_PUSH
    if (m->inotify_watchers)
    {
        msg(D_MULTI_DEBUG, "inotify watchers count: %d\n", hash_n_elements(m->inotify_watchers));
    }
#endif
}