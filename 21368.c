static void ZRLE_ENCODE(VncState *vs, int x, int y, int w, int h,
                        int zywrle_level)
{
    int ty;

    for (ty = y; ty < y + h; ty += VNC_ZRLE_TILE_HEIGHT) {

        int tx, th;

        th = MIN(VNC_ZRLE_TILE_HEIGHT, y + h - ty);

        for (tx = x; tx < x + w; tx += VNC_ZRLE_TILE_WIDTH) {
            int tw;
            ZRLE_PIXEL *buf;

            tw = MIN(VNC_ZRLE_TILE_WIDTH, x + w - tx);

            buf = zrle_convert_fb(vs, tx, ty, tw, th, ZRLE_BPP);
            ZRLE_ENCODE_TILE(vs, buf, tw, th, zywrle_level);
        }
    }
}