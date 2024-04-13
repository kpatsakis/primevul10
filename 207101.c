void qxl_render_update(PCIQXLDevice *qxl)
{
    QXLCookie *cookie;

    qemu_mutex_lock(&qxl->ssd.lock);

    if (!runstate_is_running() || !qxl->guest_primary.commands ||
        qxl->mode == QXL_MODE_UNDEFINED) {
        qxl_render_update_area_unlocked(qxl);
        qemu_mutex_unlock(&qxl->ssd.lock);
        graphic_hw_update_done(qxl->ssd.dcl.con);
        return;
    }

    qxl->guest_primary.commands = 0;
    qxl->render_update_cookie_num++;
    qemu_mutex_unlock(&qxl->ssd.lock);
    cookie = qxl_cookie_new(QXL_COOKIE_TYPE_RENDER_UPDATE_AREA,
                            0);
    qxl_set_rect_to_surface(qxl, &cookie->u.render.area);
    qxl_spice_update_area(qxl, 0, &cookie->u.render.area, NULL,
                          0, 1 /* clear_dirty_region */, QXL_ASYNC, cookie);
}