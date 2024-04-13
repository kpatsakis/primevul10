void vnc_zlib_clear(VncState *vs)
{
    if (vs->zlib.stream.opaque) {
        deflateEnd(&vs->zlib.stream);
    }
    buffer_free(&vs->zlib.zlib);
}