static void vnc_display_close(VncDisplay *vd)
{
    if (!vd) {
        return;
    }
    vd->is_unix = false;

    if (vd->listener) {
        qio_net_listener_disconnect(vd->listener);
        object_unref(OBJECT(vd->listener));
    }
    vd->listener = NULL;

    if (vd->wslistener) {
        qio_net_listener_disconnect(vd->wslistener);
        object_unref(OBJECT(vd->wslistener));
    }
    vd->wslistener = NULL;

    vd->auth = VNC_AUTH_INVALID;
    vd->subauth = VNC_AUTH_INVALID;
    if (vd->tlscreds) {
        object_unparent(OBJECT(vd->tlscreds));
        vd->tlscreds = NULL;
    }
    if (vd->tlsauthz) {
        object_unparent(OBJECT(vd->tlsauthz));
        vd->tlsauthz = NULL;
    }
    g_free(vd->tlsauthzid);
    vd->tlsauthzid = NULL;
    if (vd->lock_key_sync) {
        qemu_remove_led_event_handler(vd->led);
        vd->led = NULL;
    }
#ifdef CONFIG_VNC_SASL
    if (vd->sasl.authz) {
        object_unparent(OBJECT(vd->sasl.authz));
        vd->sasl.authz = NULL;
    }
    g_free(vd->sasl.authzid);
    vd->sasl.authzid = NULL;
#endif
}