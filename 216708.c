init_management_callback_multi(struct multi_context *m)
{
#ifdef ENABLE_MANAGEMENT
    if (management)
    {
        struct management_callback cb;
        CLEAR(cb);
        cb.arg = m;
        cb.flags = MCF_SERVER;
        cb.status = management_callback_status;
        cb.show_net = management_show_net_callback;
        cb.kill_by_cn = management_callback_kill_by_cn;
        cb.kill_by_addr = management_callback_kill_by_addr;
        cb.delete_event = management_delete_event;
        cb.n_clients = management_callback_n_clients;
#ifdef MANAGEMENT_DEF_AUTH
        cb.kill_by_cid = management_kill_by_cid;
        cb.client_auth = management_client_auth;
        cb.get_peer_info = management_get_peer_info;
#endif
#ifdef MANAGEMENT_PF
        cb.client_pf = management_client_pf;
#endif
        management_set_callback(management, &cb);
    }
#endif /* ifdef ENABLE_MANAGEMENT */
}