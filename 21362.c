static void vnc_dpy_switch(DisplayChangeListener *dcl,
                           DisplaySurface *surface)
{
    static const char placeholder_msg[] =
        "Display output is not active.";
    static DisplaySurface *placeholder;
    VncDisplay *vd = container_of(dcl, VncDisplay, dcl);
    bool pageflip = vnc_check_pageflip(vd->ds, surface);
    VncState *vs;

    if (surface == NULL) {
        if (placeholder == NULL) {
            placeholder = qemu_create_message_surface(640, 480, placeholder_msg);
        }
        surface = placeholder;
    }

    vnc_abort_display_jobs(vd);
    vd->ds = surface;

    /* guest surface */
    qemu_pixman_image_unref(vd->guest.fb);
    vd->guest.fb = pixman_image_ref(surface->image);
    vd->guest.format = surface->format;

    if (pageflip) {
        vnc_set_area_dirty(vd->guest.dirty, vd, 0, 0,
                           surface_width(surface),
                           surface_height(surface));
        return;
    }

    /* server surface */
    vnc_update_server_surface(vd);

    QTAILQ_FOREACH(vs, &vd->clients, next) {
        vnc_colordepth(vs);
        vnc_desktop_resize(vs);
        if (vs->vd->cursor) {
            vnc_cursor_define(vs);
        }
        memset(vs->dirty, 0x00, sizeof(vs->dirty));
        vnc_set_area_dirty(vs->dirty, vd, 0, 0,
                           vnc_width(vd),
                           vnc_height(vd));
        vnc_update_throttle_offset(vs);
    }
}