static void vnc_desktop_resize(VncState *vs)
{
    if (vs->ioc == NULL || !vnc_has_feature(vs, VNC_FEATURE_RESIZE)) {
        return;
    }
    if (vs->client_width == pixman_image_get_width(vs->vd->server) &&
        vs->client_height == pixman_image_get_height(vs->vd->server)) {
        return;
    }

    assert(pixman_image_get_width(vs->vd->server) < 65536 &&
           pixman_image_get_width(vs->vd->server) >= 0);
    assert(pixman_image_get_height(vs->vd->server) < 65536 &&
           pixman_image_get_height(vs->vd->server) >= 0);
    vs->client_width = pixman_image_get_width(vs->vd->server);
    vs->client_height = pixman_image_get_height(vs->vd->server);
    vnc_lock_output(vs);
    vnc_write_u8(vs, VNC_MSG_SERVER_FRAMEBUFFER_UPDATE);
    vnc_write_u8(vs, 0);
    vnc_write_u16(vs, 1); /* number of rects */
    vnc_framebuffer_update(vs, 0, 0, vs->client_width, vs->client_height,
                           VNC_ENCODING_DESKTOPRESIZE);
    vnc_unlock_output(vs);
    vnc_flush(vs);
}