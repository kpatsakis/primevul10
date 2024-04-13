static void vnc_zlib_start(VncState *vs)
{
    buffer_reset(&vs->zlib.zlib);

    // make the output buffer be the zlib buffer, so we can compress it later
    vs->zlib.tmp = vs->output;
    vs->output = vs->zlib.zlib;
}