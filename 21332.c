static bool check_solid_tile(VncState *vs, int x, int y, int w, int h,
                             uint32_t* color, bool samecolor)
{
    QEMU_BUILD_BUG_ON(VNC_SERVER_FB_BYTES != 4);
    return check_solid_tile32(vs, x, y, w, h, color, samecolor);
}