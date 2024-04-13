static void vnc_disconnect_start(VncState *vs)
{
    if (vs->disconnecting) {
        return;
    }
    trace_vnc_client_disconnect_start(vs, vs->ioc);
    vnc_set_share_mode(vs, VNC_SHARE_MODE_DISCONNECTED);
    if (vs->ioc_tag) {
        g_source_remove(vs->ioc_tag);
        vs->ioc_tag = 0;
    }
    qio_channel_close(vs->ioc, NULL);
    vs->disconnecting = TRUE;
}