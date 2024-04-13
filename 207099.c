void qxl_render_update_area_bh(void *opaque)
{
    PCIQXLDevice *qxl = opaque;

    qemu_mutex_lock(&qxl->ssd.lock);
    qxl_render_update_area_unlocked(qxl);
    qemu_mutex_unlock(&qxl->ssd.lock);
}