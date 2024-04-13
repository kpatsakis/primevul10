static void framebuffer_update_request(VncState *vs, int incremental,
                                       int x, int y, int w, int h)
{
    if (incremental) {
        if (vs->update != VNC_STATE_UPDATE_FORCE) {
            vs->update = VNC_STATE_UPDATE_INCREMENTAL;
        }
    } else {
        vs->update = VNC_STATE_UPDATE_FORCE;
        vnc_set_area_dirty(vs->dirty, vs->vd, x, y, w, h);
    }
}