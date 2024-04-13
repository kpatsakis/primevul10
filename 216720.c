multi_add_iroutes(struct multi_context *m,
                  struct multi_instance *mi)
{
    struct gc_arena gc = gc_new();
    const struct iroute *ir;
    const struct iroute_ipv6 *ir6;
    if (TUNNEL_TYPE(mi->context.c1.tuntap) == DEV_TYPE_TUN)
    {
        mi->did_iroutes = true;
        for (ir = mi->context.options.iroutes; ir != NULL; ir = ir->next)
        {
            if (ir->netbits >= 0)
            {
                msg(D_MULTI_LOW, "MULTI: internal route %s/%d -> %s",
                    print_in_addr_t(ir->network, 0, &gc),
                    ir->netbits,
                    multi_instance_string(mi, false, &gc));
            }
            else
            {
                msg(D_MULTI_LOW, "MULTI: internal route %s -> %s",
                    print_in_addr_t(ir->network, 0, &gc),
                    multi_instance_string(mi, false, &gc));
            }

            mroute_helper_add_iroute46(m->route_helper, ir->netbits);

            multi_learn_in_addr_t(m, mi, ir->network, ir->netbits, false);
        }
        for (ir6 = mi->context.options.iroutes_ipv6; ir6 != NULL; ir6 = ir6->next)
        {
            msg(D_MULTI_LOW, "MULTI: internal route %s/%d -> %s",
                print_in6_addr(ir6->network, 0, &gc),
                ir6->netbits,
                multi_instance_string(mi, false, &gc));

            mroute_helper_add_iroute46(m->route_helper, ir6->netbits);

            multi_learn_in6_addr(m, mi, ir6->network, ir6->netbits, false);
        }
    }
    gc_free(&gc);
}