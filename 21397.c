static int vnc_display_listen(VncDisplay *vd,
                              SocketAddress **saddr,
                              size_t nsaddr,
                              SocketAddress **wsaddr,
                              size_t nwsaddr,
                              Error **errp)
{
    size_t i;

    if (nsaddr) {
        vd->listener = qio_net_listener_new();
        qio_net_listener_set_name(vd->listener, "vnc-listen");
        for (i = 0; i < nsaddr; i++) {
            if (qio_net_listener_open_sync(vd->listener,
                                           saddr[i], 1,
                                           errp) < 0)  {
                return -1;
            }
        }

        qio_net_listener_set_client_func(vd->listener,
                                         vnc_listen_io, vd, NULL);
    }

    if (nwsaddr) {
        vd->wslistener = qio_net_listener_new();
        qio_net_listener_set_name(vd->wslistener, "vnc-ws-listen");
        for (i = 0; i < nwsaddr; i++) {
            if (qio_net_listener_open_sync(vd->wslistener,
                                           wsaddr[i], 1,
                                           errp) < 0)  {
                return -1;
            }
        }

        qio_net_listener_set_client_func(vd->wslistener,
                                         vnc_listen_io, vd, NULL);
    }

    return 0;
}