static int vnc_refresh_lossy_rect(VncDisplay *vd, int x, int y)
{
    VncState *vs;
    int sty = y / VNC_STAT_RECT;
    int stx = x / VNC_STAT_RECT;
    int has_dirty = 0;

    y = QEMU_ALIGN_DOWN(y, VNC_STAT_RECT);
    x = QEMU_ALIGN_DOWN(x, VNC_STAT_RECT);

    QTAILQ_FOREACH(vs, &vd->clients, next) {
        int j;

        /* kernel send buffers are full -> refresh later */
        if (vs->output.offset) {
            continue;
        }

        if (!vs->lossy_rect[sty][stx]) {
            continue;
        }

        vs->lossy_rect[sty][stx] = 0;
        for (j = 0; j < VNC_STAT_RECT; ++j) {
            bitmap_set(vs->dirty[y + j],
                       x / VNC_DIRTY_PIXELS_PER_BIT,
                       VNC_STAT_RECT / VNC_DIRTY_PIXELS_PER_BIT);
        }
        has_dirty++;
    }

    return has_dirty;
}