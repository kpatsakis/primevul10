size_t vnc_client_io_error(VncState *vs, ssize_t ret, Error **errp)
{
    if (ret <= 0) {
        if (ret == 0) {
            trace_vnc_client_eof(vs, vs->ioc);
            vnc_disconnect_start(vs);
        } else if (ret != QIO_CHANNEL_ERR_BLOCK) {
            trace_vnc_client_io_error(vs, vs->ioc,
                                      errp ? error_get_pretty(*errp) :
                                      "Unknown");
            vnc_disconnect_start(vs);
        }

        if (errp) {
            error_free(*errp);
            *errp = NULL;
        }
        return 0;
    }
    return ret;
}