check_solid_tile32(VncState *vs, int x, int y, int w, int h,
                   uint32_t *color, bool samecolor)
{
    VncDisplay *vd = vs->vd;
    uint32_t *fbptr;
    uint32_t c;
    int dx, dy;

    fbptr = vnc_server_fb_ptr(vd, x, y);

    c = *fbptr;
    if (samecolor && (uint32_t)c != *color) {
        return false;
    }

    for (dy = 0; dy < h; dy++) {
        for (dx = 0; dx < w; dx++) {
            if (c != fbptr[dx]) {
                return false;
            }
        }
        fbptr = (uint32_t *)
            ((uint8_t *)fbptr + vnc_server_fb_stride(vd));
    }

    *color = (uint32_t)c;
    return true;
}