multi_init(struct multi_context *m, struct context *t, bool tcp_mode, int thread_mode)
{
    int dev = DEV_TYPE_UNDEF;

    msg(D_MULTI_LOW, "MULTI: multi_init called, r=%d v=%d",
        t->options.real_hash_size,
        t->options.virtual_hash_size);

    /*
     * Get tun/tap/null device type
     */
    dev = dev_type_enum(t->options.dev, t->options.dev_type);

    /*
     * Init our multi_context object.
     */
    CLEAR(*m);

    m->thread_mode = thread_mode;

    /*
     * Real address hash table (source port number is
     * considered to be part of the address).  Used
     * to determine which client sent an incoming packet
     * which is seen on the TCP/UDP socket.
     */
    m->hash = hash_init(t->options.real_hash_size,
                        get_random(),
                        mroute_addr_hash_function,
                        mroute_addr_compare_function);

    /*
     * Virtual address hash table.  Used to determine
     * which client to route a packet to.
     */
    m->vhash = hash_init(t->options.virtual_hash_size,
                         get_random(),
                         mroute_addr_hash_function,
                         mroute_addr_compare_function);

    /*
     * This hash table is a clone of m->hash but with a
     * bucket size of one so that it can be used
     * for fast iteration through the list.
     */
    m->iter = hash_init(1,
                        get_random(),
                        mroute_addr_hash_function,
                        mroute_addr_compare_function);

#ifdef MANAGEMENT_DEF_AUTH
    m->cid_hash = hash_init(t->options.real_hash_size,
                            0,
                            cid_hash_function,
                            cid_compare_function);
#endif

#ifdef ENABLE_ASYNC_PUSH
    /*
     * Mapping between inotify watch descriptors and
     * multi_instances.
     */
    m->inotify_watchers = hash_init(t->options.real_hash_size,
                                    get_random(),
                                    int_hash_function,
                                    int_compare_function);
#endif

    /*
     * This is our scheduler, for time-based wakeup
     * events.
     */
    m->schedule = schedule_init();

    /*
     * Limit frequency of incoming connections to control
     * DoS.
     */
    m->new_connection_limiter = frequency_limit_init(t->options.cf_max,
                                                     t->options.cf_per);

    /*
     * Allocate broadcast/multicast buffer list
     */
    m->mbuf = mbuf_init(t->options.n_bcast_buf);

    /*
     * Different status file format options are available
     */
    m->status_file_version = t->options.status_file_version;

    /*
     * Possibly allocate an ifconfig pool, do it
     * differently based on whether a tun or tap style
     * tunnel.
     */
    if (t->options.ifconfig_pool_defined)
    {
        int pool_type = IFCONFIG_POOL_INDIV;

        if (dev == DEV_TYPE_TUN && t->options.topology == TOP_NET30)
        {
            pool_type = IFCONFIG_POOL_30NET;
        }

        m->ifconfig_pool = ifconfig_pool_init(pool_type,
                                              t->options.ifconfig_pool_start,
                                              t->options.ifconfig_pool_end,
                                              t->options.duplicate_cn,
                                              t->options.ifconfig_ipv6_pool_defined,
                                              t->options.ifconfig_ipv6_pool_base,
                                              t->options.ifconfig_ipv6_pool_netbits );

        /* reload pool data from file */
        if (t->c1.ifconfig_pool_persist)
        {
            ifconfig_pool_read(t->c1.ifconfig_pool_persist, m->ifconfig_pool);
        }
    }

    /*
     * Help us keep track of routing table.
     */
    m->route_helper = mroute_helper_init(MULTI_CACHE_ROUTE_TTL);

    /*
     * Initialize route and instance reaper.
     */
    m->reaper = multi_reap_new(reap_buckets_per_pass(t->options.virtual_hash_size));

    /*
     * Get local ifconfig address
     */
    CLEAR(m->local);
    ASSERT(t->c1.tuntap);
    mroute_extract_in_addr_t(&m->local, t->c1.tuntap->local);

    /*
     * Per-client limits
     */
    m->max_clients = t->options.max_clients;

    m->instances = calloc(m->max_clients, sizeof(struct multi_instance *));

    /*
     * Initialize multi-socket TCP I/O wait object
     */
    if (tcp_mode)
    {
        m->mtcp = multi_tcp_init(t->options.max_clients, &m->max_clients);
    }
    m->tcp_queue_limit = t->options.tcp_queue_limit;

    /*
     * Allow client <-> client communication, without going through
     * tun/tap interface and network stack?
     */
    m->enable_c2c = t->options.enable_c2c;

    /* initialize stale routes check timer */
    if (t->options.stale_routes_check_interval > 0)
    {
        msg(M_INFO, "Initializing stale route check timer to run every %i seconds and to removing routes with activity timeout older than %i seconds",
            t->options.stale_routes_check_interval, t->options.stale_routes_ageing_time);
        event_timeout_init(&m->stale_routes_check_et, t->options.stale_routes_check_interval, 0);
    }

    m->deferred_shutdown_signal.signal_received = 0;
}