static void vnc_zrle_stop(VncState *vs)
{
    /* switch back to normal output/zlib buffers */
    vs->zrle->zrle = vs->output;
    vs->output = vs->zrle->tmp;
}