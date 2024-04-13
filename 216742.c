multi_del_iroutes(struct multi_context *m,
                  struct multi_instance *mi)
{
    const struct iroute *ir;
    const struct iroute_ipv6 *ir6;
    if (TUNNEL_TYPE(mi->context.c1.tuntap) == DEV_TYPE_TUN)
    {
        for (ir = mi->context.options.iroutes; ir != NULL; ir = ir->next)
        {
            mroute_helper_del_iroute46(m->route_helper, ir->netbits);
        }

        for (ir6 = mi->context.options.iroutes_ipv6; ir6 != NULL; ir6 = ir6->next)
        {
            mroute_helper_del_iroute46(m->route_helper, ir6->netbits);
        }
    }
}