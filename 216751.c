multi_process_float(struct multi_context *m, struct multi_instance *mi)
{
    struct mroute_addr real;
    struct hash *hash = m->hash;
    struct gc_arena gc = gc_new();

    if (!mroute_extract_openvpn_sockaddr(&real, &m->top.c2.from.dest, true))
    {
        goto done;
    }

    const uint32_t hv = hash_value(hash, &real);
    struct hash_bucket *bucket = hash_bucket(hash, hv);

    /* make sure that we don't float to an address taken by another client */
    struct hash_element *he = hash_lookup_fast(hash, bucket, &real, hv);
    if (he)
    {
        struct multi_instance *ex_mi = (struct multi_instance *) he->value;

        struct tls_multi *m1 = mi->context.c2.tls_multi;
        struct tls_multi *m2 = ex_mi->context.c2.tls_multi;

        /* do not float if target address is taken by client with another cert */
        if (!cert_hash_compare(m1->locked_cert_hash_set, m2->locked_cert_hash_set))
        {
            msg(D_MULTI_LOW, "Disallow float to an address taken by another client %s",
                multi_instance_string(ex_mi, false, &gc));

            mi->context.c2.buf.len = 0;

            goto done;
        }

        msg(D_MULTI_MEDIUM, "closing instance %s", multi_instance_string(ex_mi, false, &gc));
        multi_close_instance(m, ex_mi, false);
    }

    msg(D_MULTI_MEDIUM, "peer %" PRIu32 " (%s) floated from %s to %s",
        mi->context.c2.tls_multi->peer_id,
        tls_common_name(mi->context.c2.tls_multi, false),
        mroute_addr_print(&mi->real, &gc),
        print_link_socket_actual(&m->top.c2.from, &gc));

    /* remove old address from hash table before changing address */
    ASSERT(hash_remove(m->hash, &mi->real));
    ASSERT(hash_remove(m->iter, &mi->real));

    /* change external network address of the remote peer */
    mi->real = real;
    generate_prefix(mi);

    mi->context.c2.from = m->top.c2.from;
    mi->context.c2.to_link_addr = &mi->context.c2.from;

    /* inherit parent link_socket and link_socket_info */
    mi->context.c2.link_socket = m->top.c2.link_socket;
    mi->context.c2.link_socket_info->lsa->actual = m->top.c2.from;

    tls_update_remote_addr(mi->context.c2.tls_multi, &mi->context.c2.from);

    ASSERT(hash_add(m->hash, &mi->real, mi, false));
    ASSERT(hash_add(m->iter, &mi->real, mi, false));

#ifdef MANAGEMENT_DEF_AUTH
    ASSERT(hash_add(m->cid_hash, &mi->context.c2.mda_context.cid, mi, true));
#endif

done:
    gc_free(&gc);
}