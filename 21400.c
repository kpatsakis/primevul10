int vnc_init_func(void *opaque, QemuOpts *opts, Error **errp)
{
    Error *local_err = NULL;
    char *id = (char *)qemu_opts_id(opts);

    assert(id);
    vnc_display_init(id, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        return -1;
    }
    vnc_display_open(id, &local_err);
    if (local_err != NULL) {
        error_propagate(errp, local_err);
        return -1;
    }
    return 0;
}