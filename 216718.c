multi_ifconfig_pool_persist(struct multi_context *m, bool force)
{
    /* write pool data to file */
    if (m->ifconfig_pool
        && m->top.c1.ifconfig_pool_persist
        && (force || ifconfig_pool_write_trigger(m->top.c1.ifconfig_pool_persist)))
    {
        ifconfig_pool_write(m->top.c1.ifconfig_pool_persist, m->ifconfig_pool);
    }
}