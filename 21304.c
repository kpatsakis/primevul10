static void vnc_listen_io(QIONetListener *listener,
                          QIOChannelSocket *cioc,
                          void *opaque)
{
    VncDisplay *vd = opaque;
    bool isWebsock = listener == vd->wslistener;

    qio_channel_set_name(QIO_CHANNEL(cioc),
                         isWebsock ? "vnc-ws-server" : "vnc-server");
    qio_channel_set_delay(QIO_CHANNEL(cioc), false);
    vnc_connect(vd, cioc, false, isWebsock);
}