void qxl_render_update_area_done(PCIQXLDevice *qxl, QXLCookie *cookie)
{
    qemu_mutex_lock(&qxl->ssd.lock);
    trace_qxl_render_update_area_done(cookie);
    qemu_bh_schedule(qxl->update_area_bh);
    qxl->render_update_cookie_num--;
    qemu_mutex_unlock(&qxl->ssd.lock);
    g_free(cookie);
}