static void vnc_jobs_bh(void *opaque)
{
    VncState *vs = opaque;

    assert(vs->magic == VNC_MAGIC);
    vnc_jobs_consume_buffer(vs);
}