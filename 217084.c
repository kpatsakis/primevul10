file_cleanup(i_ctx_t *i_ctx_p)
{
    gx_io_device *iodev = r_ptr(esp + 2, gx_io_device);

    iodev->procs.enumerate_close(r_ptr(esp + 5, file_enum));
    return 0;
}