void vnc_flush(VncState *vs)
{
    vnc_lock_output(vs);
    if (vs->ioc != NULL && vs->output.offset) {
        vnc_client_write_locked(vs);
    }
    if (vs->disconnecting) {
        if (vs->ioc_tag != 0) {
            g_source_remove(vs->ioc_tag);
        }
        vs->ioc_tag = 0;
    }
    vnc_unlock_output(vs);
}