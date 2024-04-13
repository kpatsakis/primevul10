multi_process_per_second_timers_dowork(struct multi_context *m)
{
    /* possibly reap instances/routes in vhash */
    multi_reap_process(m);

    /* possibly print to status log */
    if (m->top.c1.status_output)
    {
        if (status_trigger(m->top.c1.status_output))
        {
            multi_print_status(m, m->top.c1.status_output, m->status_file_version);
        }
    }

    /* possibly flush ifconfig-pool file */
    multi_ifconfig_pool_persist(m, false);

#ifdef ENABLE_DEBUG
    gremlin_flood_clients(m);
#endif

    /* Should we check for stale routes? */
    if (m->top.options.stale_routes_check_interval && stale_route_check_trigger(m))
    {
        check_stale_routes(m);
    }
}