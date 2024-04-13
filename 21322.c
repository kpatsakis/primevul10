gboolean vnc_client_io(QIOChannel *ioc G_GNUC_UNUSED,
                       GIOCondition condition, void *opaque)
{
    VncState *vs = opaque;

    assert(vs->magic == VNC_MAGIC);
    if (condition & G_IO_IN) {
        if (vnc_client_read(vs) < 0) {
            /* vs is free()ed here */
            return TRUE;
        }
    }
    if (condition & G_IO_OUT) {
        vnc_client_write(vs);
    }

    if (vs->disconnecting) {
        if (vs->ioc_tag != 0) {
            g_source_remove(vs->ioc_tag);
        }
        vs->ioc_tag = 0;
    }
    return TRUE;
}